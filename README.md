# FSE_Trabalho_2

## Observações

Aparentemente está rolando um conflito com a versão do GCC 8.3 (Rasp) e a versão em que desenvolvi o trabalho (GCC 9.3). **O projeto está compilando normalmente mas com um warning** sobre o `std::vector`.

Se atente as configurações do arquivo JSON, principalmente portas dos servidores.

## Inicialização

O servidor Central deve ser inicializado antes do servidor distribuído.

## Servidor Central

`./bin/bin 0.0.0.0 10025`

## Servidor Distribuído

`./bin/bin <arquivo_de_config.json>`
