# CHIP-8 Emulator

Emulador de **CHIP-8** desenvolvido em C++.

## Requisitos

- Visual Studio Code
- Extensão **C/C++** da Microsoft
- **g++ / MinGW** configurado no PATH

## Estrutura do projeto

```text
CHIP-8/
├── main.cpp
├── Chip8.cpp
├── Chip8.hpp
├── font.hpp
├── .vscode/
│   └── tasks.json
└── output/
    └── main.exe
```

## Compilação pelo terminal do VS Code

Abra o terminal integrado do VS Code:

**Terminal → New Terminal**

Na pasta do projeto, execute:

```bash
mkdir output
```

> Execute `mkdir output` apenas se a pasta ainda não existir.

Depois compile:

```bash
g++ -Wall -Wextra -g3 main.cpp Chip8.cpp -o output/main.exe
```

## Executar

Após a compilação:

```bash
.\output\main.exe
```

## Compilar pelo VS Code

O projeto possui uma configuração em:

```text
.vscode/tasks.json
```

Para compilar diretamente pelo VS Code, use:

```text
Ctrl + Shift + B
```

Depois, execute o programa pelo terminal:

```bash
.\output\main.exe
```

## Importante

Ao compilar, **`main.cpp` e `Chip8.cpp` precisam ser incluídos**:

```bash
g++ main.cpp Chip8.cpp -o output/main.exe
```

Compilar somente:

```bash
g++ main.cpp
```

pode gerar erros como:

```text
undefined reference to `Chip8::init()'
undefined reference to `Chip8::fetch()'
```

Isso acontece porque as implementações dessas funções estão em `Chip8.cpp`.

## Fluxo básico

```text
Editar código no VS Code
        ↓
Ctrl + Shift + B
        ↓
Compilação
        ↓
output/main.exe
        ↓
.\output\main.exe
```
