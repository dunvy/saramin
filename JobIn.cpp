#include <iostream>
#include <string>       // string
#include <unistd.h>     // sleep 함수
#include <fstream>      // 파일
#include <termios.h>    // 키보드 입력
#include <sqlite3.h>    // DB
#include <vector>       // vector 함수

using std::cout, std::cin, std::endl;

class JobUser
{
    std::string userID;
    std::string userPW;
    char arr[2][2] ={
        {0,2},
        {1,3}
        };
    char arrJob[3][2] ={
        {0,2},
        {1,3},
        {1,4}
        };
    // std::vector<std::vector<std::string>> companyList;


public:
    int linux_kbhit();      // 키보드
    void PrintStart();      // 시작화면
    void StartMap();        // 시작화면_선택지
    void StartMove();       // 시작화면_선택
    void Login();           // 로그인
    void Join();            // 회원가입

    void RandomJob();       // 메인화면_랜덤공고
    void MainChoice();      // 메인화면_선택지
    void MainMove();        // 메인화면_선택
};

// 키보드
int JobUser::linux_kbhit()
{
    struct termios oldt, newt;
    int ch;
    
    tcgetattr(STDIN_FILENO, &oldt);     // 현재 터미널 설정 읽음
    newt = oldt;

    newt.c_lflag &= ~( ICANON | ECHO ); // CANONICAL과 ECHO 끔
    newt.c_cc[VMIN] = 1;                // 최소 입력 문자 수를 1로 설정
    newt.c_cc[VTIME] = 0;               // 최소 읽기 대기 시간을 0으로 설정

    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

// 시작화면
void JobUser::PrintStart()
{
    int userChoice;
    system("clear");
    std::string line;
    std::ifstream startLogo("logo.txt");
    if (startLogo.is_open())
    {        
        while (!startLogo.eof())
        {    
            getline(startLogo, line);
            cout << line << endl;   
        }
    }
    startLogo.close();

    cout<<"┌───────────────────────────────────────────────────────┐"<<endl;
    cout<<"│                                                       │" << endl;
    cout<<"│         본 서비스는 로그인 후 이용 가능합니다.        │" << endl;
    cout<<"│                                                       │" << endl;
    cout<<"└───────────────────────────────────────────────────────┘"<<endl;
}

// 시작페이지 출력
void JobUser::StartMap()
{
    PrintStart();
    cout<<"─────────────────────────────────────────────────────────"<<endl;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (arr[i][j] == 0)
                cout<<"                      ➡️";
            if (arr[i][j] == 1)
                cout<<"                        ";
            if (arr[i][j] == 2)
                cout<<" 로그인";
            if (arr[i][j] == 3)
                cout<<" 회원가입";
        }
        cout<<endl;
    }
    cout<<"─────────────────────────────────────────────────────────"<<endl;
}

// 위아래 키보드 움직이기
void JobUser::StartMove()
{
    int key = 0, xpos, ypos, temp;
    StartMap();

    while (1)
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (arr[i][j] == 0)
                {
                    xpos = i;
                    ypos = j;
                }
            }
        }
        key = linux_kbhit();
    
        if (key == 65)  // 위
        {
            if (xpos != 0)
            {
                temp = arr[xpos][ypos];
                arr[xpos][ypos] = arr[xpos-1][ypos];
                arr[xpos-1][ypos] = temp;
            }
        }
        else if (key == 66)
        {
            if (xpos != 1)
            {
                temp = arr[xpos][ypos];
                arr[xpos][ypos] = arr[xpos+1][ypos];
                arr[xpos+1][ypos] = temp;
            }
        }
        else if (key == 10)  // 엔터
        {
            if (xpos == 0)
            {
                Login();
                break;
            }
            else if(xpos == 1)
            {
                Join();
                break;
            }
        }
        else
            continue;
        StartMove();
    }
}

// 로그인
void JobUser::Login()
{
    // 데이터베이스 핸들
    sqlite3 *db;
    // SQL 쿼리 핸들
    sqlite3_stmt *res;    
    // 에러메세지
    char *err_msg = 0;
    // 함수 반환값 저장
    int rc;
    int count = 0;

    // 데이터베이스 열기
    rc = sqlite3_open("joinDB.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "파일오픈 실패: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    while (1)
    {
        system("clear");
        cout<<"┌───────────────────────────────────────────────────────┐"<<endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                   로그인 진행 중👻                    │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                 아이디를 입력해주세요!                │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"└───────────────────────────────────────────────────────┘"<<endl;
        cout << "아이디 입력: ";
        cin>>userID;

        cout<<"┌───────────────────────────────────────────────────────┐"<<endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                   로그인 진행 중👻                    │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                비밀번호를 입력해주세요!               │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"└───────────────────────────────────────────────────────┘"<<endl;
        cout << "비밀번호 입력: ";
        cin>>userPW;

        std::string QUERY_USERLOGIN = "SELECT userID, userPW FROM Users WHERE userID = '" + userID + "' AND userPW ='" + userPW + "'";
        sqlite3_stmt *res;

        rc = sqlite3_prepare_v2(db, QUERY_USERLOGIN.c_str(), -1, &res, 0);
        rc = sqlite3_step(res);
        if (rc != SQLITE_ROW)
        {
            if (count == 2)
            {
                cout<<"\n로그인 3회 틀리셨습니다🙏"<<endl<<"엔터를 누르면 시작 화면으로 돌아갑니다."<<endl;
                cin.clear();
                cin.ignore(100, '\n');
                cin.get();
                StartMove();
                break;
            }
            else
            {
                cout<<"\n아이디 또는 비밀번호가 일치하지 않습니다🙏 \n로그인 3회 틀릴 시 시작 화면으로 돌아갑니다.\n(엔터를 눌러 재입력 진행)"<<endl;
                cin.clear();
                cin.ignore(100, '\n');
                cin.get();
                count++;
                continue;
            }
        }
        break;
    }
}

// 회원가입 (중복검사)
void JobUser::Join()
{
    // 데이터베이스 핸들
    sqlite3 *db;
    // SQL 쿼리 핸들
    sqlite3_stmt *res;
    // 에러메세지
    char *err_msg = 0;
    // 함수 반환값 저장
    int rc;

    // 데이터베이스 열기
    rc = sqlite3_open("joinDB.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "파일오픈 실패: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    std::string id, pw, phone, email;
    // 비밀번호 숫자만 입력받기 위한 변수
    int ipw;

    // 아이디
    while(1)
    {
        system("clear");
        cout<<"┌───────────────────────────────────────────────────────┐"<<endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                   회원가입 진행 중😋                  │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                 아이디를 입력해주세요!                │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"└───────────────────────────────────────────────────────┘"<<endl;

        cout << "아이디 입력: ";
        cin >> id;
        
        // 쿼리에 값 전달
        std::string QUERY_ID = "SELECT userID FROM Users WHERE userID = '" + id + "'";
        sqlite3_stmt *res;
        rc = sqlite3_prepare_v2(db, QUERY_ID.c_str(), -1, &res, 0);

        // 반환값이 SQLITE_ROW 이면 쿼리 결과로 행(row)하나가 준비됨
        rc = sqlite3_step(res);
        if (rc == SQLITE_ROW)
        {
            cout<<"중복된 아이디입니다🙏 (엔터를 눌러 재입력 진행)"<<endl;
            cin.clear();
            cin.ignore(100, '\n');
            cin.get();
        }
        else
            break;
    }
    

    // 비밀번호
    while (1)
    {
        system("clear");
        cout<<"┌───────────────────────────────────────────────────────┐"<<endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                   회원가입 진행 중😋                  │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                비밀번호를 입력해주세요!               │" << endl;
        cout<<"│          (첫번째 숫자는 0을 제외한 숫자 입력)         │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"└───────────────────────────────────────────────────────┘"<<endl;

        cout<< "핀 번호 입력(숫자 4자리): ____\b\b\b\b";
        cin>>ipw;

        // 숫자만 입력받기
        if (!cin)
        {
            cout<< "숫자만 입력해주세요🙏 (엔터를 눌러 재입력 진행)";
            cin.clear();
            cin.ignore(100, '\n');
            cin.get();
            continue;
        }

        // int를 string으로 형 변환
        pw = std::to_string(ipw);

        if (pw.length() != 4)
        {
            cout<< "4글자 입력해주세요🙏 (엔터를 눌러 재입력 진행)";
            cin.clear();
            cin.ignore(100, '\n');
            cin.get();
            continue;
        }
        break;
    }


    // 핸드폰번호
    while(1)
    {
        system("clear");
        cout<<"┌───────────────────────────────────────────────────────┐"<<endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                   회원가입 진행 중😋                  │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"│              휴대폰 번호를 입력해주세요!              │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"└───────────────────────────────────────────────────────┘"<<endl;

        cout<< "핸드폰 번호 입력(숫자 11자리): ___________\b\b\b\b\b\b\b\b\b\b\b";
        cin>>phone;

        if (phone.length() != 11)
        {
            cout<< "11글자 입력해주세요🙏 (엔터를 눌러 재입력 진행)";
            cin.clear();
            cin.ignore(100, '\n');
            cin.get();
            continue;
        }
        else
        {
            std::string QUERY_PHONE = "SELECT userPhone FROM Users WHERE userPhone = '" + phone + "'";
            sqlite3_stmt *res;
            rc = sqlite3_prepare_v2(db, QUERY_PHONE.c_str(), -1, &res, 0);

            rc = sqlite3_step(res);
            if (rc == SQLITE_ROW)
            {
                cout<<"중복된 휴대폰 번호입니다🙏 (엔터를 눌러 재입력 진행)"<<endl;
                cin.clear();
                cin.ignore(100, '\n');
                cin.get();
                continue;
            }
            break;
        }       
    }


    // 이메일
    while(1)
    {
        system("clear");
        cout<<"┌───────────────────────────────────────────────────────┐"<<endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                   회원가입 진행 중😋                  │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"│                 이메일을 입력해주세요!                │" << endl;
        cout<<"│                                                       │" << endl;
        cout<<"└───────────────────────────────────────────────────────┘"<<endl;

        cout << "이메일 입력: ";
        cin >> email;

        if (email.find("@") == std::string::npos)
        {
            cout<<"이메일 형식이 맞지 않습니다."<<endl;
            sleep(1);
            continue;
        }

        std::string QUERY_EMAIL = "SELECT userEmail FROM Users WHERE userEmail = '" + email + "'";
        sqlite3_stmt *res;
        rc = sqlite3_prepare_v2(db, QUERY_EMAIL.c_str(), -1, &res, 0);

        rc = sqlite3_step(res);
        if (rc == SQLITE_ROW)
        {
            cout<<"중복된 이메일입니다🙏 (엔터를 눌러 재입력 진행)"<<endl;
            cin.clear();
            cin.ignore(100, '\n');
            cin.get();
            continue;
        }
        cin.ignore();   // 엔터 초기화
        break;
    }

    // 데이터 삽입
    std::string INSERT_SQL = "INSERT INTO Users (userID, userPW, userPhone, userEmail) VALUES('" + id + "', " + pw + ", " + phone + ", '" + email + "')";
    // SQL 실행
    rc = sqlite3_exec(db, INSERT_SQL.c_str(), 0,0, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL INSERT error: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }

    cout<<"\n회원 가입이 완료됐습니다." << endl;
    sleep(1);
    cout<<endl<<"반갑습니다 " << id << "님!🥰 (3초 후 시작 화면으로 돌아갑니다.)\n";
    sleep(1);
    cout<<endl;
    for (int i = 3; i > 0; i--)
    {
        cout<<i<<"..."<<endl;
        sleep(1);
    }   
        
    // 데이터베이스 닫기
    sqlite3_close(db);
    StartMove();
}

// 공고 랜덤
void JobUser::RandomJob()
{
    // 데이터베이스 핸들
    sqlite3 *db;
    // SQL 쿼리 핸들
    sqlite3_stmt *res;
    // 에러메세지
    char *err_msg = 0;
    // 함수 반환값 저장
    int rc;

    std::vector<std::vector<std::string>> companyList;

    // 데이터베이스 열기
    rc = sqlite3_open("DBCompany", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "파일오픈 실패: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    std::string QUERY_RANDOM = "SELECT 회사명, 공고제목, 경력, 주요스킬, 근무지역, 접수기간 FROM companyDB ORDER BY RANDOM() LIMIT 3";

    rc = sqlite3_prepare_v2(db, QUERY_RANDOM.c_str(), -1, &res, 0);
    while (sqlite3_step(res) == SQLITE_ROW)
    {   
        // 한 행에 해당하는 값을 담을 벡터 선언
        std::vector<std::string> temp;
        // 한 행의 컬럼 개수만큼 벡터에 담기
        for (size_t i = 0; i < sqlite3_column_count(res); i++)
        {
            std::string str = (const char*)sqlite3_column_text(res, i);
            temp.push_back(str);
        }
        companyList.push_back(temp);
    }
        
    // prepare 리셋
    sqlite3_reset(res);
    // res 리셋
    sqlite3_finalize(res);

    cout<<"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"<<endl;
    for(size_t i = 0; i < companyList.size(); i++)
    {
        for (size_t j = 0; j < companyList[0].size(); j++)
        {
            if (j == 0)
                cout<<"◾ 회사명: ";
            else if (j == 1)
                cout<<"◾ 공고제목: ";
            else if (j == 2)
                cout<<"◾ 경력: ";
            else if (j == 3)
                cout<<"◾ 주요스킬: ";
            else if (j == 4)
                cout<<"◾ 근무지역: ";
            else if (j == 5)
                cout<<"◾ 접수기간: ";
            
            cout<<companyList[i][j]<<endl;
        }
        if (i != companyList.size()-1)
        {
            cout<<"─────────────────────────────────────────────────────────"<<endl;
        }    
    }
    cout<<"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"<<endl;    
}

// 메인페이지 출력
void JobUser::MainChoice()
{
    RandomJob();
    cout<<"─────────────────────────────────────────────────────────"<<endl;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (arrJob[i][j] == 0)
                cout<<"                      ➡️";
            if (arrJob[i][j] == 1)
                cout<<"                        ";
            if (arrJob[i][j] == 2)
                cout<<" 기업 추천 리스트 선택하기";
            if (arrJob[i][j] == 3)
                cout<<" 검색";
            if (arrJob[i][j] == 4)
                cout<<" 종료";
        }
        cout<<endl;
    }
    cout<<"─────────────────────────────────────────────────────────"<<endl;
}

void JobUser::MainMove()
{
    int key = 0, xpos, ypos, temp;
    // 메인화면 출력
    MainChoice();

    while (1)
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (arr[i][j] == 0)
                {
                    xpos = i;
                    ypos = j;
                }
            }
        }
        key = linux_kbhit();
    
        if (key == 65)  // 위
        {
            if (xpos != 0)
            {
                temp = arr[xpos][ypos];
                arr[xpos][ypos] = arr[xpos-1][ypos];
                arr[xpos-1][ypos] = temp;
            }
        }
        else if (key == 66) // 아래
        {
            if (xpos != 2)
            {
                temp = arr[xpos][ypos];
                arr[xpos][ypos] = arr[xpos+1][ypos];
                arr[xpos+1][ypos] = temp;
            }
        }
        else if (key == 10)  // 엔터
        {
            if (xpos == 0)
            {
                
                break;
            }
            else if(xpos == 1)
            {
                
                break;
            }
            else if(xpos == 2)
            {
                
                break;
            }
        }
        else
            continue;
        MainMove();
    }
}

int main(void)
{
    JobUser user;

    // user.StartMove();

    user.MainMove();

    return 0;
}