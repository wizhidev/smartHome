cmake_minimum_required(VERSION 3.7)
project(shome_lib)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES json/cJSON.c
        incharge/format_bin_to_json.c global.c
        data_list/gw_list.c json/s2j.c json/s2j.h json/s2jdef.h
        pthread/locker_pthread.c pthread/locker_pthread.h
        pthread/locker.h comm/comm.h data_list/gw_node.c data_list/gw_node.h
        incharge/to_gwclient.c incharge/to_gwclient.h data_list/snPkt.c data_list/snPkt.h udp/udp.c udp/udp.h
        comm/log.c comm/syslog.c  udp/network.c pthread/pthread_pool.c
        incharge/extract_data.c incharge/extract_data.h incharge/to_uiclient.c
        incharge/to_uiclient.h comm/str_comm.h comm/str_comm.c comm/udppropkt.h
        incharge/format_json_to_bin.c incharge/format_json_to_bin.h
        time_task/timer.c )

add_executable(shome_lib ${SOURCE_FILES})