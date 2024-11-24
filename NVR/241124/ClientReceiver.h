#ifndef CLIENTRECEIVER_H
#define CLIENTRECEIVER_H

#include "Storage.h"

/**
 * ReceiveAndStoreFrames
 * - Edge에서 데이터를 수신하고, Storage에 저장하는 함수.
 *
 * @param storage Storage 객체 참조
 * @param port 클라이언트 소켓이 수신할 포트 번호
 */
void ReceiveAndStoreFrames(video::Storage& storage, int port);

#endif // CLIENTRECEIVER_H

