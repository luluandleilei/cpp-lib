
//本地头文件

//系统头文件

//标准库头文件

#include <string>
#include <vector>
#include <assert.h>


static const int MAX_PATH_LEN = 4096;





/*
 * @brief  获取当前程序的绝对路径
 *
 * @param  self_path(out) 成功将本程序实际路径全名放入self_path中
 * @return 成功返回true，失败false
 */      
bool get_self_path(string *self_path) {
  assert(self_path);
  char buf[MAX_PATH_LEN] = { 0 };
  if (readlink("/proc/self/exe", buf, sizeof(buf)) < 0) {
    return false;
  }
  self_path->assign(buf);
  return true;
}

/**
 * @brief  获取当前工作目录
 *
 * @param  cwd(out) 获取当前工作目录
 * @return 成功返回true，失败false
 */
bool get_current_working_dir(string *cwd) {
  assert(cwd);
  char path[MAX_PATH_LEN] = { 0 };
  char *ptr = getcwd(path, MAX_PATH_LEN);
  if (!ptr) {
    return false;
  }
  cwd->assign(ptr);
  return true;
}

/*
 * @brief  去掉path末尾多余的斜杠'/'
 *
 * @param  path(in) 待过滤的路径
 * @return 返回过滤末尾'/'后的path
 */
string trim_back_slash(const string &path) {
  string::size_type pos = path.length() - 1;
  while (pos != string::npos && path[pos] == '/') {
    --pos;
  }
  return path.substr(0, pos + 1);
}

/*
 * @brief  去掉path头部多余的斜杠'/'
 *
 * @param  path(in) 待过滤的路径
 * @return 返回过滤头部'/'后的path
 */
string trim_front_slash(const string &path) {
  string::size_type pos = 0;
  while (pos < path.length() && path[pos] == '/') {
    ++pos;
  }
  return path.substr(pos);
}

/*
 * @brief 分离给定path中父路径和最后一级路径名
 *
 * @param  path(in)         给定的路径
 * @param  parent_path(out) path的父路径
 * @param  short_name (out) path的最后一级路径
 * @return 成功true，失败false
 */
bool path_slipt(const string &path, string *parent_path, string *short_name) {
  assert(parent_path && short_name);
  string tmp_path = trim_back_slash(path);
  string::size_type pos = tmp_path.rfind('/');
  if (pos != string::npos && pos != tmp_path.length() - 1) {
    *parent_path = 0 == pos ? "/" : tmp_path.substr(0, pos);
    short_name->assign(tmp_path.substr(pos + 1));
    return true;
  }
  return false;
}

/*
 * @brief  取得指定目录的父目录
 *
 * @param  path(in)  给定目录
 * @return 存在父目录则返回父目录，否则返回空串
 */
string parent_path(const string &path) {
  string tmp_path = trim_back_slash(path);
  string::size_type pos = tmp_path.rfind('/');
  if (pos != string::npos) {
    return 0 == pos ? "/" : tmp_path.substr(0, pos);
  }
  return string();
}

/*
 * @brief  取得某个路径的最后一级路径名
 *
 * @param  path(in) 源文件路径
 * @return 返回最后一级路径名
 */
string file_name(const string &path) {
  string tmp_path = trim_back_slash(path);
  string::size_type pos = tmp_path.rfind('/');
  if (pos != string::npos) {
    return tmp_path.substr(pos + 1);
  } else {
    return tmp_path;
  }
}

/*
 * @brief  取得某个路径的子路径，即subpath("abc/def") == "def"
 *
 * @param  path(in) 源文件路径
 * @return 返回子路径名
 */
string subpath(const string &path) {
  string tmp_path = trim_front_slash(path);
  string::size_type pos = tmp_path.find('/');
  if (pos != string::npos) {
    return tmp_path.substr(pos + 1);
  } else {
    return string();
  }
}

/*
 * @brief  获取当前进程用户的$HOME目录
 *
 * @param  home(out) 成功则返回$HOME目录
 * @return 成功返回true，失败返回false
 */
bool get_home_path(string *home_path) {
  assert(home_path);
  char *path = getenv("HOME");
  if (!path) {
    return false;
  }
  home_path->assign(path);
  return true;
}

/*
 * @brief  取得当前工作根目录
 *  假定的目录结构为$root/bin/program，根据program找到$root
 *
 * @param  root_path 成功返回根目录
 * @return 成功返回true，失败返回false
 */
bool get_root_path(string *root_path) {
  assert(root_path);
  string program;
  if (!get_self_path(&program)) {
    return false;
  }
  root_path->assign(parent_path(parent_path(program)));
  if (root_path->empty()) {
    return false;
  }
  return true;
}

/**
 * @brief  补全为绝对路径
 *
 * @param  path(in) 某个给定路径
 * @return 转换为绝对路径，即如果是相对目录，则用当前工作目录补全
 */
string complete_full_path(const string &path) {
  string full_path = path;
  if (path.empty() || path[0] != '/') {
    string cwd;
    bool ret = get_current_working_dir(&cwd);
    assert(ret);
    full_path = cwd + '/' + full_path;
  }
  return full_path;
}

/**
 * @brief  给路径中插入反斜杠,以适应正则表达式, 如/a/b/c转换成\/a\/b\/c
 *
 * C字符串反斜杠会被转移，所以实际上是代码中加了两个反斜杠
 * @param  path(in) 某个路径
 * @return 转换后的path
 */
string insert_back_slash(const string &path) {
  string tmp;
  string::size_type start = 0;
  string::size_type pos;
  while ((pos = path.find('/', start)) != string::npos) {
    tmp.append(path.substr(start, pos - start)).append("\\/");
    start = pos + 1;
  }
  tmp.append(path.substr(start));
  return tmp;
}

/*
 * @brief  判断给定路径是否为绝对路径
 *
 * @param  path 某个路径
 * @return true: 是绝对路径; false: 不是绝对路径
 */
bool is_absolute_path(const string &path) {
  if (path[0] == '/') {
    return true;
  }
  return false;
}

/**
 * @brief  整理用户输入的路径，去掉多余的/ . ..
 *
 * @param  path(in) 源路径
 * @return 如果是正确的路径则返回过滤后的，否则返回空
 */
string trim_path(const string &path) {
  if (path.empty()) {
    return path;
  }
  std::vector<string> v;
  if (path[0] == '/') {
    v.push_back(string("/"));
  }
  string tmp;
  string::size_type start = 0;
  string::size_type pos;
  do {
    pos = path.find('/', start);
    tmp = path.substr(start, pos - start);
    if (tmp.empty() || tmp == ".") {
    } else if (tmp == "..") {
      if (v.size() <= 1) {
        return string();
      }
      v.pop_back();
    } else {
      v.push_back(tmp);
    }
    start = pos + 1;
  } while (pos != string::npos);
  assert(!v.empty());
  tmp = v[0] == "/" ? "" : v[0];
  size_t v_len = v.size();
  for (size_t i = 1; i != v_len; ++i) {
    tmp.append("/").append(v[i]);
  }
  return tmp;
}

/**
 * @brief  检查给定目录是否存在，不存在则递归创建
 *
 * @param  path(in)  目录
 * @param  mode(in)  创建目录的权限
 * @return 成功返回true，失败返回false
 */
bool check_and_mkdir(const string &path, mode_t mode) {
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0) {  // 没有该路径则创建
    if (mkdir(path.c_str(), mode) != 0) {
      return false;
    }
  } else if (!(statbuf.st_mode & S_IFDIR)) {
    return false;
  }
  return true;
}

/**
 * @brief  检查给定目录是否存在，不存在则递归创建
 *
 * @param  path(in)  目录
 * @param  mode(in)  创建目录的权限
 * @return 成功返回true，失败返回false
 */
bool check_and_mkdir_r(const string &path, mode_t mode) {
  string parent = parent_path(path);
  if (!parent.empty()) {
    struct stat statbuf;
    if (stat(parent.c_str(), &statbuf) != 0) {
      if (!check_and_mkdir_r(parent, mode)) {
        return false;
      }
    } else if (!(statbuf.st_mode & S_IFDIR)) {
      return false;
    }
  }
  return check_and_mkdir(path, mode);
}
