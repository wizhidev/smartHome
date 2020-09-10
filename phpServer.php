<?php

$UNIX_DOMAIN_SERVER_PATH = "/tmp/unix_domain_path_server.sock";

$msg = "msg";
$socket = socket_create(AF_UNIX, SOCK_DGRAM, 0);
socket_connect($socket, $UNIX_DOMAIN_SERVER_PATH);

$sockfile = '/tmp/unix_domain_path_client.sock';

$server = stream_socket_server("unix://$sockfile", $errno, $errstr);

if (!$server)
{
        die("创建unix domain socket fail: $errno - $errstr");
}
socket_send($socket, $msg, strlen($msg), 0);

while(1)  
{
        $conn = stream_socket_accept($server, 5);

        if ($conn)
        {
                while(1)
                {
                    $msg = fread($conn, 1024);
                    if (strlen($msg) == 0) //客户端关闭
                    {
                        fclose($conn);
                        break;
                    }
                    echo "read data: $msg";
                    fwrite($conn, "read ok!");
                }
        }

}
fclose($server);