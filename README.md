# zephyr_mcu_boot

Aplicacao Zephyr para a **nRF54L15 DK**, configurada com MCUboot e atualizacao
de firmware (DFU) por serial. A aplicacao de exemplo pisca o LED da placa.

O projeto oferece dois caminhos seriais diferentes:

- **MCUmgr/UART na aplicacao:** para consultar e atualizar a imagem enquanto a
  aplicacao esta em execucao.
- **Recuperacao serial do MCUboot:** modo de recuperacao do bootloader, iniciado
  pelo botao configurado na placa.

## Requisitos

- nRF Connect SDK com Zephyr e toolchain instalados. O build deste projeto foi
  verificado com NCS v3.4.0.
- nRF54L15 DK e cabo USB com acesso a porta serial virtual (VCOM).
- `west`, disponibilizado pelo ambiente/toolchain do NCS.

## Build e gravacao

Na raiz do projeto, em um terminal configurado pelo nRF Connect SDK:

```powershell
west build -b nrf54l15dk/nrf54l15/cpuapp --sysbuild -p always -d build_54dk .
west flash -d build_54dk
```

`--sysbuild` inclui MCUboot no build. O artefato de atualizacao para
distribuicao e gerado em `build_54dk/dfu_application.zip`. A imagem assinada da
aplicacao tambem fica em
`build_54dk/zephyr_mcu_boot/zephyr/zephyr.signed.bin`.

## DFU usando MCUmgr pela UART

O transporte MCUmgr/UART esta habilitado em `prj.conf`. A configuracao da placa
seleciona a UART MCUmgr no devicetree; use a porta COM virtual correspondente a
essa UART. O terminal serial deve usar **115200 baud, 8-N-1**, sem controle de
fluxo.

Com a aplicacao em execucao, use uma ferramenta MCUmgr para conectar-se a porta
serial. Por exemplo, com o CLI `mcumgr`:

```powershell
mcumgr --conntype serial --connstring "dev=COMx,baud=115200" image list
mcumgr --conntype serial --connstring "dev=COMx,baud=115200" image upload build_54dk/zephyr_mcu_boot/zephyr/zephyr.signed.bin
```

Substitua `COMx` pela porta da placa. Depois do upload, use o hash retornado
pela ferramenta para marcar a nova imagem para teste e reinicie:

```powershell
mcumgr --conntype serial --connstring "dev=COMx,baud=115200" image test <hash>
mcumgr --conntype serial --connstring "dev=COMx,baud=115200" reset
```

Quando a nova imagem iniciar corretamente, confirme-a para que o MCUboot a
mantenha como imagem ativa:

```powershell
mcumgr --conntype serial --connstring "dev=COMx,baud=115200" image confirm
```

Se a imagem de teste nao for confirmada, o MCUboot pode reverter para a imagem
anterior no proximo reinicio. Feche outros programas que estejam usando a
mesma porta serial antes de executar os comandos.

## Recuperacao serial do MCUboot

O MCUboot tambem esta compilado com recuperacao serial UART. No overlay, o
alias `mcuboot-button0` aponta para o segundo botao da placa (`button1`).

Para entrar no modo de recuperacao:

1. Feche o terminal ou qualquer ferramenta que esteja usando a porta serial.
2. Mantenha pressionado o segundo botao (BTN1) da DK.
3. Enquanto o mantem pressionado, reinicie ou ligue a placa.
4. Solte o botao quando o bootloader entrar no modo serial.
5. Conecte a ferramenta de recuperacao a porta COM virtual e envie o pacote
   gerado em `build_54dk/dfu_application.zip`, seguindo o fluxo da ferramenta.

Este modo pertence ao bootloader e e diferente do transporte MCUmgr ativo
durante a execucao normal da aplicacao.

## Configuracao relevante

- `sysbuild.conf`: inclui MCUboot e configura slots de imagem.
- `sysbuild/mcuboot.conf`: habilita o transporte serial UART no bootloader e
  desabilita o console UART do MCUboot.
- `sysbuild/mcuboot.overlay`: associa o botao e o LED de indicacao usados pelo
  MCUboot.
- `prj.conf`: habilita MCUmgr, grupo de imagens e transporte UART na aplicacao.
