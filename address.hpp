#pragma once
#include <mysql/mysql.h>
#include <iostream>
using namespace std;


class address_book
{
  public:
    //构造函数
    address_book()
    {
      _con = mysql_init(NULL);
      if(_con == NULL) {
        std::cout << "ERROR:" << mysql_error(_con) << std::endl;

      }
    }

    //连接数据库
    //bool initDB(string host, string user, string passwd, string db_name);
    bool initDB();

    //添加
    bool add();

    //显示
    bool print();

    //查询
    void select();

    //修改
    bool modify();

    //排序
    bool order();

    //菜单
    void menu();

    //测试
    void test();

    //析构函数
    ~address_book()
    {
      if(_con != NULL) {
        mysql_close(_con);
      }

    }
  private:
    MYSQL* _con; 

    //查询
    bool _select(string& name);   
};


//bool address_book::initDB(string host, string user, string passwd, string db_name)
//{
//  _con = mysql_real_connect(_con, host.c_str(), user.c_str(), passwd.c_str(), db_name.c_str(), 0, NULL, 0);
//  if(_con == NULL) {
//    cout << "Rrror:" << mysql_error(_con);
//    return false;
//  }
//  return true;
//}

bool address_book::initDB()
{
  _con = mysql_real_connect(_con, "localhost","root", "123456", "address_book", 0, NULL, 0);

  mysql_query(_con, "set character set utf8");
  if(_con == NULL) {
    cout << "Rrror:" << mysql_error(_con) << endl;
    return false;
  }
  return true;
}

bool address_book::add()
{
  string name;
  string address;
  string tel;
  string postal_code;
  cout << "请输入姓名，地址，电话，邮编：" <<endl;
  cin >> name >> address >> tel >> postal_code;
  string sql = "insert into contact(name, address, tel, postal_code) values('" + name + "','" + address + "','" + tel + "','" + postal_code + "');";
  if(mysql_query(_con, sql.c_str())) {
    cout << "插入失败  Error:" << mysql_error(_con) <<endl;
    return false;
  }
  else {
    cout << "添加成功" << endl;
  }
  return true;
}

bool address_book::print()
{
  MYSQL_ROW row;
  string sql = "select * from contact;";
  if(mysql_query(_con, sql.c_str())) {
    cout << "Error: " << mysql_error(_con) << endl;
    return false;
  }
  else {
    MYSQL_RES* result = mysql_store_result(_con);
    int num_fields = mysql_num_fields(result);
    int num_rows = mysql_num_rows(result);
    for(int i = 0; i < num_rows; ++i) {
      row = mysql_fetch_row(result);
      if(row) {
        break;
      }
      for(int j = 0; j < num_fields;++j) {
        cout << row[j] << " ";
      }
      cout << endl;
    } 
    mysql_free_result(result);  
  }
  return true;
}

void address_book::select()
{
  cout << "请输入需要查询的姓名：" << endl;
  string name;
  cin >> name;
  _select(name);
  return;
}

bool address_book::_select(string& name)
{

  string sql = "select * from contact where name = '" + name + "';";
  if(mysql_query(_con, sql.c_str())) {
    cout << "查询失败 Error: " << mysql_error(_con) << endl;
    return false;
  }
  else {
    MYSQL_ROW row;
    MYSQL_RES* result = mysql_use_result(_con);
    int num_fields = mysql_num_fields(result);
    int num_rows = mysql_num_rows(result);
    for(int i = 0; i < num_rows; ++i) {
      row = mysql_fetch_row(result);
      if(row) {
        break;
      }
      for(int j = 0; j < num_fields; ++j) {
        cout << row[j] << " ";
      }
      cout << endl;
    } 
    mysql_free_result(result);  
  }
  return true;
}

bool address_book::modify()
{
  cout << "请输入需要修改联系人的姓名：" << endl;
  string name;
  cin >> name;
  if(!_select(name)) {
    return false;
  }
  else {
    cout << "输入改正后的电话：" << endl;
    string tel;
    cin >> tel;
    string sql = "update contact set tel = '" + tel + "' where name = '" + name + "';";
    if(mysql_query(_con, sql.c_str())) {
      cout << mysql_error(_con) << endl;
      return false;
    }
  }
  cout << "修改成功！" << endl;
  return true;
}

void address_book::menu()
{
  std::cout << "****************" << std::endl;
  std::cout << "\t1.添加\n" << std::endl;
  std::cout << "\t2.显示\t" << std::endl;
  std::cout << "\t3.查询\t" << std::endl;
  std::cout << "\t4.修改\t" << std::endl;
  std::cout << "\t5.排序\t" << std::endl;
  std::cout << "****************" << std::endl;
}

void address_book::test()
{
  bool k = initDB();
  cout << k << endl;
  while(true) {
    menu();
    cout << "请输入选项：" << endl;
    string chose;
    cin >> chose;
    if(chose.compare("1") == 0) {
      add();
    }
    else if(chose.compare("2") == 0) {
      print();
    }
    else if(chose.compare("3") == 0) {
      select();
    }
    else if(chose.compare("4") == 0) {
      modify();
    }
    else if(chose.compare("5") == 0) {

    }
    else {
      cout << "输入有误！" << endl;
    }
  }
}
