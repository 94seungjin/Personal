#include <iostream>  // 표준 입출력 라이브러리
#include <fstream>   // 파일 입출력 라이브러리
#include <vector>    // 동적 배열을 위한 벡터 라이브러리

// H.264 파일에서 Header와 Body를 분리하여 Body만 저장하는 함수
void extractBodyFromH264(const std::string& inputFile, const std::string& outputFile)
{
    // 입력 파일 열기 (바이너리 모드)
    std::ifstream input(inputFile, std::ios::binary);
    if (!input.is_open()) {  // 파일 열기 실패 시 오류 메시지 출력 후 종료
        std::cerr << "Error: Unable to open input file: " << inputFile << std::endl;
        return;
    }

    // 입력 파일 내용을 메모리에 읽어들임
    std::vector<char> fileData((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();  // 파일 스트림 닫기

    // H.264 파일의 Body 시작점을 찾기 위한 Start Code 정의
    const std::string startCode = "\x00\x00\x00\x01"; // H.264에서 사용되는 일반적인 NAL Unit 시작 코드
    // Header와 Body를 구분하기 위해 Start Code 이후의 위치를 탐색
    size_t bodyStartPos = std::string(fileData.begin(), fileData.end()).find(startCode, 4);
    // find(startCode, 4): 첫 번째 NAL Unit 이후(보통 Header)에 위치한 Body 시작점을 찾음

    if (bodyStartPos == std::string::npos) {  // Body 시작점을 찾지 못한 경우
        std::cerr << "Error: Unable to find body start position in file: " << inputFile << std::endl;
        return;
    }

    // Body 데이터를 추출 (Start Code 이후의 모든 데이터)
    std::vector<char> bodyData(fileData.begin() + bodyStartPos, fileData.end());

    // 출력 파일 열기 (바이너리 모드)
    std::ofstream output(outputFile, std::ios::binary);
    if (!output.is_open()) {  // 파일 열기 실패 시 오류 메시지 출력 후 종료
        std::cerr << "Error: Unable to open output file: " << outputFile << std::endl;
        return;
    }

    // Body 데이터를 출력 파일에 저장
    output.write(bodyData.data(), bodyData.size());
    output.close();  // 파일 스트림 닫기

    // 작업 완료 메시지 출력
    std::cout << "Body data extracted and saved to: " << outputFile << std::endl;
}

int main() {
    // 입력 파일 이름 (현재 디렉토리에 있는 H.264 파일)
    std::string inputFile = "192.168.50.14.h264";
    // 출력 파일 이름 (Body만 저장된 H.264 파일)
    std::string outputFile = "output_body_1.h264";

    // H.264 파일에서 Body 추출 함수 호출
    extractBodyFromH264(inputFile, outputFile);

    return 0;  // 프로그램 종료
}
