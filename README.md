# Compilação

- Instalar gcc e make: ``sudo apt install build-essentials``

- No diretório do projeto, rodar ``make``, um binário chamado ``edger`` surgirá.

# Utilizando ``edger``

- Uso: ``./edger <caminho-do-arquivo>``

Sendo ``<nome-do-arquivo>`` o arquivo contendo o grafo.

Qualquer divergência da estrutura do arquivo em relação a amostra do enunciado,
resultará em um comportamento não definido.

# Exemplo de uso

Existem algumas amostras dentro do diretório ``samples``.

``./edger ./samples/basic.txt``
