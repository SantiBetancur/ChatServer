# Proyecto 1: Sistema de Chat en Red

## Introducción
El proyecto consiste en una aplicación de chat cliente-servidor desarrollada en C que permite a múltiples usuarios comunicarse en tiempo real a través de una red. El sistema implementa un modelo de comunicación donde los clientes se conectan a un servidor central que gestiona la autenticación de usuarios y la distribución de mensajes.

Este proyecto demuestra conceptos fundamentales de programación en red, manejo de hilos (threads), autenticación de usuarios y comunicación en tiempo real utilizando sockets TCP/IP. La aplicación está diseñada con una arquitectura modular que separa claramente las responsabilidades del cliente y del servidor.

## Desarrollo

### Arquitectura del Sistema

El proyecto está dividido en dos componentes principales:

1. **Servidor**: Responsable de:
   - Autenticación de usuarios
   - Gestión de conexiones de clientes
   - Distribución de mensajes entre clientes
   - Registro de actividad (logging)

2. **Cliente**: Proporciona:
   - Interfaz para autenticación
   - Envío y recepción de mensajes

### Estructura del Proyecto

```
├── Client
│   ├── include
│   │   ├── client_common.h
│   │   └── connection.h
│   └── src
│       ├── client_main.c
│       └── connection.c
├── Server
│   ├── include
│   │   ├── auth.h
│   │   ├── client_handler.h
│   │   └── server_common.h
│   └── src
│       ├── auth.c
│       ├── client_handler.c
│       ├── server_common.c
│       └── server_main.c
├── build
├── config
│   └── users.txt
├── Makefile
├── .gitignore
└── README.md
```

### Implementación

#### Servidor

El servidor implementa un modelo multihilo donde cada cliente conectado es manejado por un hilo independiente. Las principales funcionalidades son:

1. **Autenticación de Usuarios**: Verifica las credenciales de los usuarios contra un archivo de configuración (`config/users.txt`) o permite el acceso a un usuario predeterminado cuando este archivo no existe.

2. **Gestión de Clientes**: Mantiene una lista de clientes conectados y proporciona mecanismos para añadir, eliminar y comunicarse con ellos.

3. **Difusión de Mensajes**: Distribuye los mensajes recibidos a todos los demás clientes conectados.

4. **Logging**: Registra eventos importantes como conexiones, desconexiones y mensajes en un archivo de registro.

#### Cliente

El cliente proporciona una interfaz de usuario simple basada en terminal con las siguientes características:

1. **Autenticación**: Solicita al usuario sus credenciales y las envía al servidor para verificación.

2. **Interfaz de Chat**: Muestra mensajes entrantes y permite al usuario enviar mensajes al grupo.

3. **Hilos Paralelos**: Utiliza hilos separados para enviar y recibir mensajes, permitiendo una comunicación bidireccional simultánea.

### Flujo de Comunicación

1. El cliente inicia una conexión con el servidor.
2. El cliente envía credenciales de autenticación.
3. El servidor verifica las credenciales y responde con éxito o fallo.
4. Si la autenticación es exitosa, el servidor crea un hilo dedicado para manejar al cliente.
5. El cliente crea dos hilos: uno para enviar mensajes y otro para recibir.
6. Cuando un cliente envía un mensaje, el servidor lo distribuye a todos los demás clientes.
7. Cuando un cliente se desconecta, el servidor notifica a los demás clientes.

## Despliegue del servidor en la maquina virtual
### Configuración del Entorno AWS
Para el despliegue del servidor se utilizó una instancia EC2 de AWS Academy con las siguientes características:

- Sistema Operativo: Ubuntu Server 22.04 LTS
- Tipo de Instancia: t2.micro (1 vCPU, 1GB RAM)
- Almacenamiento: 8GB SSD
- Grupo de Seguridad: Configurado para permitir conexiones TCP entrantes en el puerto 8080

### Proceso de Instalación

Preparación del Entorno:
bash# Actualizar repositorios e instalar dependencias
sudo apt update
sudo apt install -y build-essential git make

Clonación del Repositorio:
bashgit clone https://github.com/SantiBetancur/ChatServer  
cd ChatServer

Configuración como Servicio Systemd
Para garantizar que el servidor se ejecute automáticamente al iniciar el sistema y se reinicie en caso de fallo, se configuró un servicio systemd.

## Aspectos Logrados y No logrados

### Logrados

1. **Arquitectura Cliente-Servidor Funcional**: Implementación completa de la comunicación a través de sockets TCP/IP utilizando la API de Sockets de Berkeley.

2. **Sistema de Autenticación**: Verificación de credenciales contra un archivo de configuración, permitiendo que los usuarios se identifiquen con un nombre de usuario al conectarse.

3. **Comunicación en Tiempo Real**: Transmisión instantánea de mensajes entre usuarios mediante sockets de tipo Stream (SOCK_STREAM) para garantizar una entrega confiable.

4. **Manejo de Múltiples Clientes**: El servidor gestiona varios clientes simultáneamente mediante hilos (pthreads), cumpliendo con el requisito de concurrencia.

5. **Notificaciones de Estado**: Notificación a todos los usuarios cuando alguien se conecta o desconecta del chat.

6. **Registro de Actividad**: Registro de eventos importantes (conexiones, desconexiones y mensajes) en un archivo de log, tal como se especificaba en los requisitos del proyecto.

7. **Interfaz de Usuario Básica**: Interfaz de terminal con formato de mensajes para mejorar la legibilidad.

8. **Comandos de Salida**: Implementación de comandos especiales (/exit, /quit) para que los usuarios puedan desconectarse correctamente del servidor.

9. **Despliegue en Entorno Realista**: El servidor puede ejecutarse en una máquina virtual con sistema operativo Linux o en una instancia en la nube, cumpliendo con los requisitos técnicos del proyecto.

### No Logrados

1. **Manejo Avanzado de Reconexión**: Si bien se implementó un mecanismo básico de reconexión, el sistema no garantiza la reconexión automática sin pérdida de datos en caso de caída del servidor, como se sugería en las especificaciones.

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

## Conclusiones

El proyecto demuestra la implementación exitosa de un sistema de chat en red utilizando conceptos fundamentales de la programación de sistemas distribuidos. La arquitectura cliente-servidor proporciona una base sólida para el intercambio de mensajes en tiempo real, mientras que la estructura modular del código facilita el mantenimiento y la extensión de funcionalidades.

El uso de hilos permite una comunicación bidireccional fluida, donde los clientes pueden enviar y recibir mensajes simultáneamente sin bloqueos. El sistema de autenticación proporciona un nivel básico de seguridad, aunque podría mejorarse con técnicas de cifrado y protección de contraseñas más avanzadas.

Las principales limitaciones del sistema actual incluyen la falta de una interfaz gráfica, mensajes privados y persistencia de datos. Estas características podrían implementarse en futuras versiones para mejorar la experiencia del usuario y la funcionalidad general del sistema.

En resumen, el proyecto cumple con los objetivos básicos de un sistema de chat en red, proporcionando una plataforma funcional para la comunicación entre múltiples usuarios a través de una red TCP/IP.

## Autores
- Juan Diego Robles de la Ossa (https://github.com/juanrobles05)
- Santiago Betancur (https://github.com/SantiBetancur)
- Mateo Andres Amaya Cardona (https://github.com/maanamac)

## Referencias
1. https://networkprogrammingnotes.blogspot.com/p/berkeley-sockets.html

2. https://docs.oracle.com/cd/E26502_01/html/E35303/tlib-1.html

3. https://medium.com/the-code-vault/systemd-simple-service-examples-ce586afeed27

4. Linux Manual Pages:
   - `socket(2)`
   - `connect(2)`
   - `bind(2)`
   - `listen(2)`
   - `accept(2)`
   - `pthread_create(3)`
   - `pthread_join(3)`