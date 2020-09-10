<?php

$msg = "msg";
$UNIX_DOMAIN_SERVER_PATH = "/tmp/unix_domain_path_server.sock";
$UNIX_DOMAIN_CLIENT_PATH = "/tmp/unix_domain_path_client.sock";

if (file_exists($UNIX_DOMAIN_CLIENT_PATH)) {
    if (!unlink($UNIX_DOMAIN_CLIENT_PATH)) {
        echo ("Error deleting $file");
    } else {
        echo ("Deleted $file");
    }
}
$socket = socket_create(AF_UNIX, SOCK_DGRAM, 0);
if (!socket) {
    die('unable to create af_unix socket');
}
socket_connect($socket, $UNIX_DOMAIN_SERVER_PATH);

if (!socket_bind($socket, $UNIX_DOMAIN_CLIENT_PATH)) {
    die('unable to bind to $server_side_socket');
}

socket_send($socket, $msg, strlen($msg), 0);

while (1) // server never exits
{
// receive query
    if (!socket_set_block($socket)) {
        die('Unable to set blocking mode for socket');
    }

    $buf = '';
    $from = '';
    echo "Ready to receive...\n";
// will block to wait client query
    $bytes_received = socket_recvfrom($socket, $buf, 65536, 0, $from);
    if ($bytes_received == -1) {
        die('An error occured while receiving from the socket');
    }

    echo "Received $buf from $from\n";

    $buf = "process client query here"; // process client query here

// send response
    if (!socket_set_nonblock($socket)) {
        die('Unable to set nonblocking mode for socket');
    }

// client side socket filename is known from client request: $from
    $len = strlen($buf);
    echo $len;
    $bytes_sent = socket_send($socket, $buf, $len, 0);
    //$bytes_sent = socket_sendto($socket, $buf, $len, 0, $from);
    if ($bytes_sent == -1) {
        die('An error occured while sending to the socket');
    } else if ($bytes_sent != $len) {
        die($bytes_sent . ' bytes have been sent instead of the ' . $len . ' bytes expected');
    }

    echo "Request processed\n";
    sleep(5);
}
