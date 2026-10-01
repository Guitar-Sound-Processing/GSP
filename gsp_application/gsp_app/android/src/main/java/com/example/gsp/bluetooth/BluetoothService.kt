package com.example.gsp.bluetooth

import android.annotation.SuppressLint
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import java.io.InputStream
import java.io.OutputStream
import java.util.UUID

class BluetoothService {

    private var socket: BluetoothSocket? = null
    private var output: OutputStream? = null
    private var input: InputStream? = null

    var onConnectionChanged: ((Boolean) -> Unit)? = null
    var onMessageReceived: ((String) -> Unit)? = null

    private val uuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    @Volatile
    private var isRunning = false

    @Volatile
    var isConnected = false
        private set

    private var workerThread: Thread? = null

    @SuppressLint("MissingPermission")
    fun connect(device: BluetoothDevice) {
        startAutoConnect { device }
    }

    @SuppressLint("MissingPermission")
    fun startAutoConnect(deviceProvider: () -> BluetoothDevice?) {
        stopAutoConnect()

        isRunning = true
        workerThread = Thread {
            var lastState: Boolean? = null

            while (isRunning) {
                val device = try {
                    deviceProvider()
                } catch (e: Exception) {
                    null
                }

                if (device == null) {
                    if (lastState != false) {
                        isConnected = false
                        lastState = false
                        onConnectionChanged?.invoke(false)
                    }
                    sleepQuietly(3000)
                    continue
                }

                try {
                    closeSocketQuietly()

                    val newSocket = device.createRfcommSocketToServiceRecord(uuid)
                    socket = newSocket
                    newSocket.connect()

                    output = newSocket.outputStream
                    input = newSocket.inputStream
                    isConnected = true

                    if (lastState != true) {
                        lastState = true
                        onConnectionChanged?.invoke(true)
                    }

                    send("fmt 1") // ask for short replies from Daisy Seed

                    listen()

                } catch (e: Exception) {
                    closeSocketQuietly()
                    if (lastState != false) {
                        isConnected = false
                        lastState = false
                        onConnectionChanged?.invoke(false)
                    }
                    sleepQuietly(3000)
                }
            }
        }.apply {
            name = "GSP-BluetoothReconnectThread"
            start()
        }
    }

    fun stopAutoConnect() {
        isRunning = false
        closeSocketQuietly()
        workerThread?.interrupt()
        workerThread = null
        if (isConnected) {
            isConnected = false
            onConnectionChanged?.invoke(false)
        }
    }

    fun send(command: String) {
        val currentOutput = output
        if (currentOutput != null && isConnected) {
            try {
                currentOutput.write((command + "\n").toByteArray())
                println("Command: $command")
            } catch (e: Exception) {
                e.printStackTrace()
                closeSocketQuietly()
            }
        } else {
            println("Disconnected: command skipped -> $command")
        }
    }

    private fun listen() {
        val buffer = ByteArray(1024)
        val sb = StringBuilder()

        while (isRunning && isConnected) {
            try {
                val bytes = input?.read(buffer) ?: -1
                if (bytes <= 0) break

                val chunk = String(buffer, 0, bytes)
                sb.append(chunk)

                while (sb.contains("\n")) {
                    val line = sb.toString().split("\n")[0].trim()
                    sb.delete(0, sb.indexOf("\n") + 1)

                    if (line.isNotEmpty()) {
                        onMessageReceived?.invoke(line)
                    }
                }
            } catch (e: Exception) {
                break
            }
        }
    }

    private fun closeSocketQuietly() {
        isConnected = false
        try {
            output?.close()
        } catch (_: Exception) {}
        try {
            input?.close()
        } catch (_: Exception) {}
        try {
            socket?.close()
        } catch (_: Exception) {}
        output = null
        input = null
        socket = null
    }

    private fun sleepQuietly(millis: Long) {
        try {
            Thread.sleep(millis)
        } catch (_: InterruptedException) {}
    }
}
