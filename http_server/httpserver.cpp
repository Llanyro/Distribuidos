#include "httpserver.h"
#include "tools.h"
#include <cstdio>
#include "utils.h"
#include <cstring>
#include <mysql/mysql.h>

bool httpServer::isConectedToDataBase() const
{
    return (this->serverPointer != nullptr);
}
httpServer::httpServer(unsigned short port) : httpServer(port, "/home/lubuntu/share/http_server", "localhost", "usuario", "Password1")
{

}
httpServer::httpServer(unsigned short port, std::string path, std::string ipDB, std::string user, std::string pass)
{
    this->files_path = path + "/html_dir";
    this->serverPointer = mysql_init(nullptr);
    mysql_real_connect(this->serverPointer, &ipDB[0], &user[0], &pass[0], "amazon_base", 3306, nullptr, 0);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (sock_fd < 0)
         printf("Error creating socket\n");
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    char option = 1;
    #ifdef WIN32
    setsockopt(sock_fd,SOL_SOCKET, SO_BROADCAST, &option ,sizeof(option));
    #else
    setsockopt(sock_fd,SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), &option,sizeof(option));
    #endif

    if (bind(sock_fd,(struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0)
          printf("ERROR on binding");
    listen(sock_fd,5);

    buildMimetypeTable();
}
std::string httpServer::getmimeType(char* file)
{
    std::string strfile=std::string(file);
    std::string ext="";
    std::string result="application/octet-stream";

    if(strfile[strfile.length()-4]=='.')
        ext=std::string(&strfile[strfile.length()-3]);
    else if(strfile[strfile.length()-5]=='.')
        ext=std::string(&strfile[strfile.length()-4]);

    std::map<std::string,std::string>::iterator mimetype= this->mimeTypes.find(ext);
    if(mimetype!=mimeTypes.end())
        result= mimetype->second;
    return result;
}
void httpServer::buildMimetypeTable()
{
    this->mimeTypes["aac"]="audio/aac";
    this->mimeTypes["abw"]="application/x-abiword";
    this->mimeTypes["arc"]="application/octet-stream";
    this->mimeTypes["avi"]="video/x-msvideo";
    this->mimeTypes["azw"]="application/vnd.amazon.ebook";
    this->mimeTypes["bin"]="application/octet-stream";
    this->mimeTypes["bz"]="application/x-bzip";
    this->mimeTypes["bz2"]="application/x-bzip2";
    this->mimeTypes["csh"]="application/x-csh";
    this->mimeTypes["css"]="text/css";
    this->mimeTypes["csv"]="text/csv";
    this->mimeTypes["doc"]="application/msword";
    this->mimeTypes["epub"]="application/epub+zip";
    this->mimeTypes["gif"]="image/gif";
    this->mimeTypes["htm"]="text/html";
    this->mimeTypes["html"]="text/html";
    this->mimeTypes["ico"]="image/x-icon";
    this->mimeTypes["ics"]="text/calendar";
    this->mimeTypes["jar"]="application/java-archive";
    this->mimeTypes["jpeg"]="image/jpeg";
    this->mimeTypes["jpg"]="image/jpeg";
    this->mimeTypes["js"]="application/javascript";
    this->mimeTypes["json"]="application/json";
    this->mimeTypes["mid"]="audio/midi";
    this->mimeTypes["midi"]="audio/midi";
    this->mimeTypes["mpeg"]="video/mpeg";
    this->mimeTypes["mpkg"]="application/vnd.apple.installer+xml";
    this->mimeTypes["odp"]="application/vnd.oasis.opendocument.presentation";
    this->mimeTypes["ods"]="application/vnd.oasis.opendocument.spreadsheet";
    this->mimeTypes["odt"]="application/vnd.oasis.opendocument.text";
    this->mimeTypes["oga"]="audio/ogg";
    this->mimeTypes["ogv"]="video/ogg";
    this->mimeTypes["ogx"]="application/ogg";
    this->mimeTypes["pdf"]="application/pdf";
    this->mimeTypes["png"]="image/png";
    this->mimeTypes["ppt"]="application/vnd.ms-powerpoint";
    this->mimeTypes["rar"]="application/x-rar-compressed";
    this->mimeTypes["rtf"]="application/rtf";
    this->mimeTypes["sh"]="application/x-sh";
    this->mimeTypes["svg"]="image/svg+xml";
    this->mimeTypes["swf"]="application/x-shockwave-flash";
    this->mimeTypes["tar"]="application/x-tar";
    this->mimeTypes["tif"]="image/tiff";
    this->mimeTypes["tiff"]="image/tiff";
    this->mimeTypes["ttf"]="font/ttf";
    this->mimeTypes["vsd"]="application/vnd.visio";
    this->mimeTypes["wav"]="audio/x-wav";
    this->mimeTypes["weba"]="audio/webm";
    this->mimeTypes["webm"]="video/webm";
    this->mimeTypes["webp"]="image/webp";
    this->mimeTypes["woff"]="font/woff";
    this->mimeTypes["woff2"]="font/woff2";
    this->mimeTypes["xhtml"]="application/xhtml+xml";
    this->mimeTypes["xls"]="application/vnd.ms-excel";
    this->mimeTypes["xml"]="application/xml";
    this->mimeTypes["xul"]="application/vnd.mozilla.xul+xml";
    this->mimeTypes["zip"]="application/zip";
    this->mimeTypes["3gp"]="video/3gpp";
    this->mimeTypes["3g2"]="video/3gpp2";
    this->mimeTypes["7z"]="application/x-7z-compressed";

}
int httpServer::waitForConnections()
{
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsock_fd = accept(sock_fd, (struct sockaddr*) &cli_addr,
                            &clilen);
    return newsock_fd;
}
void httpServer::sendContent(int newsock_fd,char* httpHeader,unsigned long int headerLen, char* content, unsigned long int contentLen)
{
    char* msg=new char[headerLen+contentLen+2];
    memcpy(msg,httpHeader,headerLen);
    msg[headerLen]='\r';
    msg[headerLen+1]='\n';
    memcpy(&(msg[headerLen]),content,contentLen);

    send(newsock_fd, msg,headerLen+contentLen+2,0);
}
void httpServer::sendFile(int newsock_fd, char* file)
{
    std::string mimetype = getmimeType(file);
    std::string filepath = this->files_path + std::string(file);
    //std::cout << filepath << std::endl;
    char* fileContent = nullptr;
    unsigned long int filelen = 0ull;
    char* httpHeader = nullptr;
    unsigned long int headerLen;

    readFile(&filepath[0], &fileContent, &filelen);
    createHeader(&httpHeader, &headerLen, "200 OK", &(mimetype[0]), filelen);
    sendContent(newsock_fd, httpHeader, headerLen, fileContent, filelen);
}
int httpServer::getHTTPParameter(std::vector<std::vector<std::string*>*> *lines,std::string parameter)
{
    for(unsigned long int i=0;i<lines->size();i++)
    {
        std::vector<std::string*> *v=(*lines)[i];

        if(((*v)[0])->compare(parameter) == 0)
            return atoi((*v)[1]->c_str());
    }
    return 0;
}
void httpServer::resolveRequests(int newsock_fd)
{
    std::vector<std::vector<std::string*>*> lines;
    readLines(newsock_fd,&lines);
    std::vector<std::string*>* v = (lines)[0];
    httpRequest_t req = getRequestType(v);
    switch(req)
        {
             case httpRequest_t::GET:
                {
                    std::string* s2 = (*v)[1];
                    if(s2->compare("/") == 0)
                        sendFile(newsock_fd,"/index.html");
                    else
                        sendFile(newsock_fd,&((*s2)[0]));
                }
                break;
            case httpRequest_t::POST:
               {
                    std::vector<std::string*> postLine;
                    int length=getHTTPParameter(&lines,"Content-Length:");
                    readPostLine(newsock_fd,&postLine,length);

                    std::string *s2=(*v)[1];
                    if(s2->compare("/login.php") == 0)
                    {
                        char* user= getFromPost(postLine,"uname");
                        char* pass= getFromPost(postLine,"psw");

                        if(validatePassword(user,pass))
                            sendFile(newsock_fd,"/services.html");

                        delete[] user;
                        delete[] pass;
                    }
                    if(s2->compare("/services.php") == 0)
                        sendFile(newsock_fd,"/services.html");
               }
               break;
        }
    #ifdef WIN32
    closesocket(newsock_fd);
    #else
    close(newsock_fd);
    #endif
    deleteLines(&lines);
}
bool httpServer::validatePassword(std::string username, std::string password)
{
    bool temp = false;
    if(httpServer::isConectedToDataBase())
    {
        Tools tool;

        std::string hash = tool.generarHash(password);
        std::string command = "select * from usuarios where user='" + username + "' and password='" + hash + "';";

        int qstate = mysql_query(this->serverPointer, &command[0]);
        std::cout << qstate << std::endl;
        if(qstate == 0)
        {
            MYSQL_ROW row;
            MYSQL_RES* res = nullptr;

            res = mysql_store_result(this->serverPointer);
            if(res != nullptr)
            {
                row = mysql_fetch_row(res);
                if(row != nullptr)
                    temp = true;
            }
        }
    }
    return temp;
}
void httpServer::closeServer()
{
    #ifdef WIN32
    closesocket(this->sock_fd);
    #else
    close(this->sock_fd);
    #endif
}
