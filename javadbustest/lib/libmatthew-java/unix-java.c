/*
 * Java Unix Sockets Library
 *
 * Copyright (c) Matthew Johnson 2005
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation, version 2 only.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details. 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * To Contact the author, please email src@matthew.ath.cx
 *
 */

#include "unix-java.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/un.h>


#ifdef __cplusplus
extern "C" {
#endif

   void handleerrno(JNIEnv *env)
   {
      if (0 == errno) return;
      int err = errno;
      if (EAGAIN == err) return; // we read 0 bytes due to a timeout
   const char* msg = strerror(err);
   jstring jmsg = (*env)->NewStringUTF(env, msg);
   jclass exc = (*env)->FindClass(env, "cx/ath/matthew/unix/UnixIOException");
   jmethodID cons = (*env)->GetMethodID(env, exc, "<init>", "(ILjava/lang/String;)V");
   jobject exo = (*env)->NewObject(env, exc, cons, err, jmsg);
   (*env)->DeleteLocalRef(env, exc);
   (*env)->DeleteLocalRef(env, jmsg);
   (*env)->Throw(env, exo);
   (*env)->DeleteLocalRef(env, exo);
}
   
/*
 * Class:     cx_ath_matthew_unix_UnixServerSocket
 * Method:    native_bind
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_cx_ath_matthew_unix_UnixServerSocket_native_1bind
  (JNIEnv *env, jobject o, jstring address, jboolean abstract)
{
   int sock = socket(PF_UNIX, SOCK_STREAM, 0);
   if (-1 == sock) { handleerrno(env); return -1; }
   const char* caddr = (*env)->GetStringUTFChars(env, address, 0);
   int slen = (*env)->GetStringUTFLength(env, address)+1;
   struct sockaddr_un *sad = malloc(sizeof(sa_family_t)+slen);
   if (abstract)  {
      char* shifted = sad->sun_path+1;
      strncpy(shifted, caddr, slen-1);
      sad->sun_path[0] = 0;
   } else
      strncpy(sad->sun_path, caddr, slen);
   (*env)->ReleaseStringUTFChars(env, address, caddr);
   sad->sun_family = AF_UNIX;
   int rv = bind(sock, (const  struct  sockaddr*) sad, sizeof(sa_family_t)+slen);
   free(sad);
   if (-1 == rv) { handleerrno(env); return -1; }
   rv = listen(sock, 10);
   if (-1 == rv) { handleerrno(env); return -1; }
   return sock;
}

/*
 * Class:     cx_ath_matthew_unix_UnixServerSocket
 * Method:    native_close
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_cx_ath_matthew_unix_UnixServerSocket_native_1close
  (JNIEnv * env, jobject o, jint sock)
{
   int rv = shutdown(sock, SHUT_RDWR);
   if (-1 == rv) { handleerrno(env); }
   rv = close(sock);
   if (-1 == rv) { handleerrno(env); }
}

/*
 * Class:     cx_ath_matthew_unix_UnixServerSocket
 * Method:    native_accept
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_cx_ath_matthew_unix_UnixServerSocket_native_1accept
  (JNIEnv * env, jobject o, jint sock)
{
   int newsock = accept(sock, NULL, NULL);
   if (-1 == newsock) handleerrno(env);
   return newsock;
}

/*
 * Class:     cx_ath_matthew_unix_UnixSocket
 * Method:    native_set_pass_cred
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_cx_ath_matthew_unix_UnixSocket_native_1set_1pass_1cred
  (JNIEnv *env, jobject o, jint sock, jboolean enable)
{
   int opt = enable;
   int rv = setsockopt(sock, SOL_SOCKET, SO_PASSCRED, &opt, sizeof(int));
   if (-1 == rv) { handleerrno(env);}
}

/*
 * Class:     cx_ath_matthew_unix_UnixSocket
 * Method:    native_connect
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_cx_ath_matthew_unix_UnixSocket_native_1connect
  (JNIEnv *env, jobject o, jstring address, jboolean abstract)
{
   int sock = socket(PF_UNIX, SOCK_STREAM, 0);
   if (-1 == sock) { handleerrno(env); return -1; }
   const char* caddr = (*env)->GetStringUTFChars(env, address, 0);
   int slen = (*env)->GetStringUTFLength(env, address)+1;
   struct sockaddr_un *sad = malloc(sizeof(sa_family_t)+slen);
   if (abstract)  {
      char* shifted = sad->sun_path+1;
      strncpy(shifted, caddr, slen-1);
      sad->sun_path[0] = 0;
   } else
      strncpy(sad->sun_path, caddr, slen);
   (*env)->ReleaseStringUTFChars(env, address, caddr);
   sad->sun_family = AF_UNIX;
   int rv = connect(sock, (const struct sockaddr*) sad, sizeof(sa_family_t)+slen);
   free(sad);
   if (-1 == rv) { handleerrno(env); return -1; }
   return sock;
}

/*
 * Class:     cx_ath_matthew_unix_UnixSocket
 * Method:    native_close
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_cx_ath_matthew_unix_UnixSocket_native_1close
  (JNIEnv *env, jobject o, jint sock)
{
   int rv = close(sock);
   if (-1 == rv) { handleerrno(env); }
}

/*
 * Class:     cx_ath_matthew_unix_USInputStream
 * Method:    native_recv
 * Signature: ([BII)I
 */
JNIEXPORT jint JNICALL Java_cx_ath_matthew_unix_USInputStream_native_1recv
  (JNIEnv *env, jobject o, jint sock, jbyteArray buf, jint offs, jint len, jint flags, jint timeout)
{
   fd_set rfds;
   struct timeval tv;
   jbyte* cbuf = (*env)->GetByteArrayElements(env, buf, NULL);
   void* recvb = cbuf + offs;
   int rv;

   if (timeout > 0) {
      FD_ZERO(&rfds);
      FD_SET(sock, &rfds);
      tv.tv_sec = 0;
      tv.tv_usec = timeout;
      rv = select(sock+1, &rfds, NULL, NULL, &tv);
      if (1 == rv) 
         rv = recv(sock, recvb, len, flags);
      if (-1 == rv) { handleerrno(env); return -1; }
      if (0 == rv) rv = -1;
      (*env)->ReleaseByteArrayElements(env, buf, cbuf, 0);
      return rv;
   } else  {
      rv = recv(sock, recvb, len, flags);
      (*env)->ReleaseByteArrayElements(env, buf, cbuf, 0);
      if (-1 == rv) { handleerrno(env); return -1; }
      return rv;
   }
}

/*
 * Class:     cx_ath_matthew_unix_USOutputStream
 * Method:    native_send
 * Signature: ([BII)V
 */
JNIEXPORT jint JNICALL Java_cx_ath_matthew_unix_USOutputStream_native_1send
  (JNIEnv *env, jobject o, jint sock, jbyteArray buf, jint offs, jint len)
{
   jbyte* cbuf = (*env)->GetByteArrayElements(env, buf, NULL);
   void* sendb = cbuf + offs;
   int rv = send(sock, sendb, len, 0);
   (*env)->ReleaseByteArrayElements(env, buf, cbuf, 0);
   if (-1 == rv) { handleerrno(env); return -1; }
   return rv;
}

#ifdef __cplusplus
}
#endif
