#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* c = _data;
    memcpy(c, &type, sizeof(uint8_t));
    c += sizeof(uint8_t);
    memcpy(c, nick.c_str(),sizeof(char) * 8);
    c += 8 * sizeof(char);
    memcpy(c, message.c_str(), sizeof(char) * 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char nameAux [8];
    char msgAux [80];

    //Reconstruir la clase usando el buffer _data
    char* aux = _data;
    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    memcpy(&nameAux, aux, 8 * sizeof(char));
    aux += 8 * sizeof(char);
    memcpy(&msgAux, aux, 80 * sizeof(char));

    nick = nameAux;
    message = msgAux;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        Socket* udpSocket;
        ChatMessage msg;
        //Recibir mensajes según tipo de mensaje
        socket.recv(msg, udpSocket);
        // LOGIN: Añadir al vector clients
        if(msg.type == ChatMessage::MessageType::LOGIN){
            std::unique_ptr<Socket> u_ptr(udpSocket);
            clients.push_back(std::move(u_ptr)); 
            printf("%s login.\n", msg.nick.c_str());
        }
        // LOGOUT: Eliminar del vector clients
        else if(msg.type == ChatMessage::MessageType::LOGOUT){
            auto it = clients.begin();
            while(it != clients.end() && !(*(*it).get() == *udpSocket)) it++;
            clients.erase(it);
            printf("%s logout.\n", msg.nick.c_str());
        }
        // MESSAGE: Reenviar mensaje a los demás clientes
        else if(msg.type == ChatMessage::MessageType::MESSAGE){
            for(int i=0; i<clients.size(); ++i){
                if(!(*(clients[i].get()) == *udpSocket))
                    socket.send(msg, (*clients[i].get()));
            }
            printf("%s send message.\n", msg.nick.c_str());
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string inString;
        std::getline(std::cin, inString);
        if(inString == "q" || inString == "Q"){
            logout();
            break;
        }
        ChatMessage em(nick, inString);
        em.type = ChatMessage::MessageType::MESSAGE;
        // Enviar el mensaje al server usando el socket
        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir mensaje
        ChatMessage em;
        socket.recv(em);

        if (em.type == ChatMessage::LOGOUT)
            break;

        //Escribir "nombre: mensaje"
        printf("%s: %s\n", em.nick.c_str(), em.message.c_str());
    }
}
