#ifndef TEST_PXATTR

#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>

#if defined(__FreeBSD__)
#include <sys/extattr.h>
#include <sys/uio.h>
#elif defined(__gnu_linux__)
#include <attr/xattr.h>
#elif defined(__APPLE__)
#include <sys/xattr.h>
#else
#error "Unknown system can't compile"
#endif

#include "pxattr.h"

namespace pxattr {

class AutoBuf {
public:
    char *buf;
    AutoBuf() : buf(0) {}
    ~AutoBuf() {if (buf) free(buf); buf = 0;}
    bool alloc(int n) 
    {
	if (buf) {
	    free(buf);
	    buf = 0;
	}
	buf = (char *)malloc(n); 
	return buf != 0;
    }
};

bool get(const string& path, nspace dom, const string& _name, string *value)
{
    string name;
    if (!sysname(dom, _name, &name)) 
	return false;

    ssize_t ret = -1;
    AutoBuf buf;

#if defined(__FreeBSD__)

#elif defined(__gnu_linux__)
    ret = getxattr(path.c_str(), name.c_str(), 0, 0);
    if (ret < 0)
	return false;
    if (!buf.alloc(ret+1)) // Don't want to mess with possible ret=0
	return false;
    ret = getxattr(path.c_str(), name.c_str(), buf.buf, ret);
#elif defined(__APPLE__)
    ret = getxattr(path.c_str(), name.c_str(), 0, 0, 0, 0);
    if (ret < 0)
	return false;
    if (!buf.alloc(ret+1)) // Don't want to mess with possible ret=0
	return false;
    ret = getxattr(path.c_str(), name.c_str(), buf.buf, ret, 0, 0);
#endif

    if (ret >= 0)
	value->assign(buf.buf, ret);
    return ret >= 0;
}

bool set(const string& path, nspace dom, const string& _name, 
	    const string& value)
{
    string name;
    if (!sysname(dom, _name, &name)) 
	return false;

    ssize_t ret = -1;

#if defined(__FreeBSD__)

#elif defined(__gnu_linux__)
    ret = setxattr(path.c_str(), name.c_str(), value.c_str(), value.length(),
		   0);
#elif defined(__APPLE__)
    ret = setxattr(path.c_str(), name.c_str(), value.c_str(), value.length(),
		   0, 0);
#endif
    return ret >= 0;
}

bool del(const string& path, nspace dom, const string& _name) 
{
    string name;
    if (!sysname(dom, _name, &name)) 
	return false;

    int ret = -1;

#if defined(__FreeBSD__)

#elif defined(__gnu_linux__)
    ret = removexattr(path.c_str(), name.c_str());
#elif defined(__APPLE__)
    ret = removexattr(path.c_str(), name.c_str(), 0);
#endif
    return ret >= 0;
}

bool list(const string& path, nspace dom, vector<string>* names)
{
    ssize_t ret = -1;
    AutoBuf buf;
#if defined(__FreeBSD__)

#elif defined(__gnu_linux__)
    ret = listxattr(path.c_str(), 0, 0);
    if (ret < 0) 
	return false;
    if (!buf.alloc(ret+1)) // Don't want to mess with possible ret=0
	return false;
    ret = listxattr(path.c_str(), buf.buf, ret);
#elif defined(__APPLE__)
    ret = listxattr(path.c_str(), 0, 0, 0);
    if (ret < 0) 
	return false;
    if (!buf.alloc(ret+1)) // Don't want to mess with possible ret=0
	return false;
    ret = listxattr(path.c_str(), buf.buf, ret, 0);
#endif

    // All systems return a 0-separated string list
    if (ret > 0) {
	int pos = 0;
	while (pos < ret) {
	    string n = string(buf.buf + pos);
	    string n1;
	    if (pxname(PXATTR_USER, n, &n1)) {
		names->push_back(n1);
	    }
	    pos += n.length() + 1;
	}
    }
    return true;
}

static const string userstring("user.");
bool sysname(nspace dom, const string& pname, string* sname)
{
    if (dom != PXATTR_USER) {
	errno = EINVAL;
	return false;
     }
    *sname = userstring + pname;
    return true;
}

bool pxname(nspace dom, const string& sname, string* pname) 
{
    if (sname.find("user.") != 0) {
	errno = EINVAL;
	return false;
    }
    *pname = sname.substr(userstring.length());
    return true;
}

} // namespace pxattr

#else // Testing / driver ->

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <iostream>

#include "pxattr.h"

static char *thisprog;
static char usage [] =
"trpxattr [-h] -n name [-v value] pathname...\n"
"trpxattr [-h] -x name pathname...\n"
"trpxattr [-h] -l pathname...\n"
"\n"
;
static void
Usage(void)
{
    fprintf(stderr, "%s: usage:\n%s", thisprog, usage);
    exit(1);
}

static int     op_flags;
#define OPT_MOINS 0x1
#define OPT_n	  0x2 
#define OPT_v	  0x4 
#define OPT_h     0x8
#define OPT_x     0x10
#define OPT_l     0x20

static void listattrs(const string& path)
{
    std::cout << "Path: " << path << std::endl;
    vector<string> names;
    if (!pxattr::list(path, pxattr::PXATTR_USER, &names)) {
	perror("pxattr::list");
	exit(1);
    }
    for (vector<string>::const_iterator it = names.begin(); 
	 it != names.end(); it++) {
	string value;
	if (!pxattr::get(path, pxattr::PXATTR_USER, *it, &value)) {
	    perror("pxattr::get");
	    exit(1);
	}
	std::cout << " " << *it << " => " << value << std::endl;
    }
}

void setxattr(const string& path, const string& name, const string& value)
{
    if (!pxattr::set(path, pxattr::PXATTR_USER, name, value)) {
	perror("pxattr::set");
	exit(1);
    }
}

void  printxattr(const string &path, const string& name)
{
    std::cout << "Path: " << path << std::endl;
    string value;
    if (!pxattr::get(path, pxattr::PXATTR_USER, name, &value)) {
	perror("pxattr::get");
	exit(1);
    }
    std::cout << " " << name << " => " << value << std::endl;
}

void delxattr(const string &path, const string& name) 
{
    if (pxattr::del(path, pxattr::PXATTR_USER, name) < 0) {
	perror("pxattr::del");
	exit(1);
    }
}


int main(int argc, char **argv)
{
  thisprog = argv[0];
  argc--; argv++;

  string name, value;

  while (argc > 0 && **argv == '-') {
    (*argv)++;
    if (!(**argv))
      /* Cas du "adb - core" */
      Usage();
    while (**argv)
      switch (*(*argv)++) {
      case 'l':	op_flags |= OPT_l; break;
      case 'x':	op_flags |= OPT_x; if (argc < 2)  Usage();
	  name = *(++argv); argc--; 
	goto b1;
      case 'n':	op_flags |= OPT_n; if (argc < 2)  Usage();
	  name = *(++argv); argc--; 
	goto b1;
      case 'v':	op_flags |= OPT_v; if (argc < 2)  Usage();
	  value = *(++argv); argc--; 
	goto b1;
      default: Usage();	break;
      }
  b1: argc--; argv++;
  }

  if (argc < 1)
    Usage();
  if (op_flags & OPT_l) {
      while (argc > 0) {
	  listattrs(*argv++);argc--;
      } 
  } else if (op_flags & OPT_n) {
      if (op_flags & OPT_v) {
	  while (argc > 0) {
	      setxattr(*argv++, name, value);argc--;
	  } 
      } else {
	  while (argc > 0) {
	      printxattr(*argv++, name);argc--;
	  } 
      }
  } else if (op_flags & OPT_x) {
      while (argc > 0) {
	  delxattr(*argv++, name);argc--;
      } 
  }
  exit(0);
}


#endif // Testing pxattr
