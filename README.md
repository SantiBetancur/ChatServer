# Proyecto 1: Sistema de Chat con Sockets en C

## Introducción
Este proyecto es una implementación de un sistema de chat funcional utilizando sockets en lenguaje C, diseñado para cumplir con los requisitos del curso de Telemática (ST0255). El sistema sigue una arquitectura cliente-servidor y utiliza el protocolo TCP/IP para garantizar una comunicación confiable entre múltiples clientes conectados simultáneamente. La implementación está enfocada en el aprendizaje de conceptos como la programación en red, la concurrencia y la comunicación cliente-servidor.

## Requisitos
- Compilador `gcc`.
- Herramienta `make` para la gestión de la compilación.
- Sistema operativo basado en Linux.
- Conexión de red para pruebas.

## Compilación y Ejecución

1. **Compilar el proyecto**
   Ejecuta el siguiente comando en la terminal para compilar tanto el cliente como el servidor:
   ```bash
   make
   ```

2. **Ejecutar el Servidor**
   Una vez compilado, puedes ejecutar el servidor con ("Ten en cuenta que debes cambiar la ip a localhost en el archivo Client/src/client_main.c en la linea 30 para que te funcione en local"):
   ```bash
   make run_server
   ```

3. **Ejecutar el Cliente**
   Ejecuta el cliente en una terminal diferente:
   ```bash
   make run_client
   ```

## Uso del Sistema

1. **Servidor:**
   - El servidor se ejecuta en un puerto definido y escucha conexiones entrantes.
   - Crea un hilo por cada cliente conectado para manejar la comunicación.
   - Retransmite mensajes a todos los clientes conectados.

2. **Cliente:**
   - Al iniciar, solicita al usuario ingresar un nombre de usuario.
   - Permite enviar mensajes al servidor, que son retransmitidos a otros clientes.
   - Los mensajes recibidos se muestran en tiempo real.
   - El cliente puede desconectarse correctamente escribiendo `exit`.

## Aspectos Logrados
- Comunicación bidireccional confiable mediante sockets TCP/IP.
- Manejo de múltiples clientes mediante hilos.

## Autores
- Juan Diego Robles de la Ossa (https://github.com/juanrobles05)
- Mateo Amaya Cardona (https://github.com/maanamac)
- Santiago Betancur (https://github.com/SantiBetancur)

## Referencias
- Documentación de la API de Sockets de Berkeley.
- "UNIX Network Programming" de W. Richard Stevens.
- Enunciado del proyecto proporcionado por el curso (marzo 2025).