#include <string.h>
#include <jni.h>
#include <android/log.h>

#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#define BUFLEN 512
#define NPACK 10



int handle_err(int socket, char* msg )
{
    LOGW(msg);
    close(socket);
    return 1;
}

double get_time_milliseconds()
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);

    double out = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
    return out;
}


jint
Java_com_example_hellojni_HelloUdp_connectionInfoToJni(JNIEnv* env,
                                                       jobject thiz,
                                                       jstring jstrIp, jint jintPort)
{

    const char *ip = (*env)->GetStringUTFChars(env, jstrIp, 0);
    int port = (int) jintPort;

    jclass cls=(*env)->GetObjectClass(env,thiz);
    jmethodID responsMethodID = (*env)->GetMethodID(env, cls, "appendToResponse", "(Ljava/lang/String;)V");
    jmethodID requestMethodID = (*env)->GetMethodID(env, cls, "appendToRequest", "(Ljava/lang/String;)V");

    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == -1)
    {
        return handle_err(s, "socket()");
    }

    if(fcntl(s, F_SETFL, 0 | O_NONBLOCK))
    {
        return handle_err(s, "fcntl");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);

    if (inet_aton(ip, &si_other.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        return handle_err(s, "inet_aton");
    }

    for (i=0; i<NPACK; i++)
    {
        LOGW("Sending packet %d\n", i);
        sprintf(buf, "This is packet %d\n", i);
        if (sendto(s, buf, BUFLEN, 0,(struct sockaddr *) &si_other, slen)==-1)
        {
            return handle_err(s, "sendto");
        }

        jstring jstrParam = (*env)->NewStringUTF(env, buf );
        (*env)->CallVoidMethod(env,thiz, requestMethodID, jstrParam);
        (*env)->DeleteLocalRef(env, jstrParam );
    }

    double start = get_time_milliseconds();

    while (1)
    {
        double now = get_time_milliseconds();
        if(now - start > 500) break; // wait for packets for 0.5 seconds

        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            LOGW("recvfrom");
            continue;
        }

        jstring jstrParam = (*env)->NewStringUTF(env, buf );
        (*env)->CallVoidMethod(env,thiz, responsMethodID, jstrParam);
        (*env)->DeleteLocalRef(env, jstrParam );

        LOGW("Received server packet from %s:%d\nData: %s\n\n",
               inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
    }

    close(s);
    LOGI("ip: %s, %d", ip, port);

    return 0;
}
