#ifndef EDGECAMERA_H
#define EDGECAMERA_H

#include <string>

/**
 * StartCameraAndSendFrames
 * - Raspberry Pi 카메라를 시작하고, 캡처한 프레임 데이터를 NVR(Client)로 전송하는 함수.
 *
 * @param serverAddress NVR(Client)의 IP 주소
 * @param serverPort NVR(Client)의 포트 번호
 */
void StartCameraAndSendFrames(const std::string& serverAddress, int serverPort);

#endif // EDGECAMERA_H

