# Chain Commands

## Definition

A Chain (or a pedal board, or rig) is a sequence of effects such that the output of a given effect is the input of the next one in the Chain. GSP 1.0.0 supports only a configurable single Chain (SISO – single input, single output), with some or all available effects included. A possible chain configuration could be something like:

Input $\rightarrow$ Compressor $\rightarrow$ Overdrive $\rightarrow$ Equalizer $\rightarrow$ Delay $\rightarrow$ Output

The number of effects in the chain and their sequence can be configured by commands, although only one instance of each effect can be used in the current GSP version. From input to output, the effect position is numbered starting from 0. In the above example, Compressor is placed in 0, Overdrive in 1, Equalizer in 2 and so on.

Like any analog gear, the GSP effects need two conditions to process the input signal:

1)	The effect must be inserted into chain.
2)	The effect must be switched on (see [Effect Switch *s*](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md#switch-parameter)).

The Effect Switch acts like a by-pass: if turned on (*s* = 1) the effect is active in chain. Otherwise, when *s* = 0, the effect is disabled and the signal is by-passed through the chain.

To include, delete, or change the effect position in the current chain, please refer to the *c* parameter in the [Effect Command](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md) documentation.

Here are some examples of chain shown by the ```all``` command:

> ->Inp->CMP->OVD->WAH->EQZ->DFB->NGT->Out-> <br>
> ->Inp->OCT->EQZ->CHS->RVB->LIM->Out-></br>

## Chain Commands:

Any Chain can be configured by using a small set or commands. Some commands were created to check the chain sequence, to create a new one and to erase the whole chain. They are:

- [New Chain](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Chain.md#new-chain)
- [Show Chain](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Chain.md#show-chain)
- [Clear Chain](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Chain.md#clear-chain)

### New Chain

Erase the current chain and start a new one with the complete set of effects. Although the previous chain is erased, all the effect parameters remain with their preceding settings.

	new

> ->Inp->CMP->OVD->PHR->OCT->SFT->DTN->WAH->EQZ->CHS->VBT->RVB->DFB->EFB->DFF->EFF->TML->VOL->LIM->NGT->Out->


### Show Chain

Prints the current chain sequence.

	all

> ->Inp->CMP->OVD->RVB->Out->

### Clear Chain

Clear the whole chain sequence (no effects left on chain)

	clr

> ->Inp->Out->

🎸 📱 GSP – Guitar Signal Processor (Resumo Estruturado)
🎯 Objetivo

Aplicativo Android para controle de pedal de efeitos de guitarra baseado em:

🎛️ Daisy Seed
📡 ESP32 (Bluetooth / WiFi)
📱 App Android (Jetpack Compose)
🧠 Arquitetura Geral
App Android ⇄ Bluetooth ⇄ ESP32 ⇄ Daisy Seed
📡 Comunicação
🔹 Tipo
Bluetooth clássico (RFCOMM)
🔹 Envio de comandos
ovd 1 0.500 0.800 1.000 1.000
🔹 Resposta (modo compacto)
->OVD (1) 0 0.500 0.800 1.000 1.000
🔹 Cadeia de efeitos
->Inp->CMP->OVD->RVB->Out->
📁 Estrutura do Projeto
com.example.gsp
│
├── bluetooth/
│   └── BluetoothService.kt
│
├── model/
│   ├── EffectParameter.kt
│   ├── EffectState.kt
│   └── Screen.kt
│
├── storage/
│   └── PresetManager.kt
│
├── ui/
│   ├── TopBar.kt
│   ├── HomeScreen.kt
│   ├── PedalScreen.kt
│   ├── EffectEditor.kt
│   └── ChainsScreen.kt
│
└── MainActivity.kt
🔧 Componentes Principais
📡 BluetoothService

Responsável por:

conexão Bluetooth
envio de comandos (send)
recepção (onMessageReceived)
status de conexão (onConnectionChanged)
🔹 Callbacks
var onMessageReceived: ((String) -> Unit)? = null
var onConnectionChanged: ((Boolean) -> Unit)? = null
🧠 MainActivity
Funções:
inicializar Bluetooth
receber dados
controlar navegação
manter estado global
🔹 Estados principais
var logText by mutableStateOf("")
var isConnected by mutableStateOf(false)
var currentScreen by mutableStateOf(Screen.HOME)
🔹 Configuração dos callbacks (IMPORTANTE)
bluetoothService.onMessageReceived = { msg ->
    runOnUiThread {
        logText = msg + "\n" + logText
    }
}

bluetoothService.onConnectionChanged = { connected ->
    runOnUiThread {
        isConnected = connected
    }
}
🖥️ Navegação
📁 Screen.kt
enum class Screen {
    HOME,
    PRESETS,
    CHAINS,
    SONGS,
    PLAYLISTS
}
🔹 Controle via MainActivity
onNavigate = { currentScreen = it }
🧭 UI Global
🔝 TopBar (todas as telas)

Funções:

navegação global
LED de conexão
🔹 Botões
Play | Pre | Rig | Sng | Plt
🔹 LED
🟢 conectado
🔴 desconectado
🔹 Implementação chave
Row(
    modifier = Modifier
        .weight(1f)
        .horizontalScroll(rememberScrollState())
)

👉 evita quebrar layout

📱 Telas
🏠 HomeScreen

Função futura:

selecionar música
aplicar chain automaticamente
🎛️ PedalScreen (Presets)

Funções:

editar parâmetros
enviar comandos
salvar presets
carregar presets
🎚️ EffectEditor

Componente reutilizável:

toggle ON/OFF
sliders
presets
🔗 ChainsScreen

Funções atuais:

visualizar cadeia
atualizar (all)
nova (new)
limpar (clr)
🔹 Parser de cadeia
fun parseChain(msg: String): List<String>
💾 Presets
Armazenamento
via SharedPreferences
formato JSON (Gson)
Local físico
/data/data/com.example.gsp/shared_prefs/
🔁 Fluxo de Dados
Usuário → UI → send()

ESP32 → BluetoothService
      → MainActivity (logText, isConnected)
      → UI
🧠 Estado Atual
✅ Implementado

✔ Bluetooth funcional
✔ Editor de efeitos
✔ Presets persistentes
✔ Navegação global
✔ TopBar com LED
✔ Chains básica
✔ Parser de cadeia

🚧 Em desenvolvimento
edição completa da chain
múltiplos efeitos
parser completo
🔮 Futuro
🎛️ pedalboard visual
🎵 Songs
📀 Playlists
📡 BLE
☁️ backup/export
⚠️ Decisões Técnicas

✔ Bluetooth clássico
✔ Jetpack Compose
✔ SharedPreferences
✔ arquitetura modular

🧠 Lições Importantes
remember só dentro de Composable
estado via mutableStateOf
listas devem ser recriadas
callbacks no onCreate()
UI separada da lógica
🎯 Próximo Passo Recomendado

👉 Evoluir ChainsScreen:

inserir efeito
remover efeito
alterar posição
🧾 Resumo Final

Você já possui:

✔ app funcional
✔ comunicação com hardware
✔ arquitetura sólida
✔ base pronta para expansão

🚀 Observação final

Quando reiniciar o chat, você pode dizer:

Estamos trabalhando no app GSP (Daisy Seed + ESP32 + Android).
Já temos Bluetooth, presets, TopBar com LED e Chains básica.
Vamos continuar a partir daqui:

👉 Isso me coloca de volta no contexto rapidamente.

Se quiser depois, posso:

👉 transformar isso em PDF
👉 criar roadmap técnico
👉 ou continuar direto da Chain avançada

Boa decisão salvar agora — você já tem um projeto bem sólido 🚀
