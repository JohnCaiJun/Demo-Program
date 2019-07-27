# add-apt-repository源码分析

>创建时间：2019-07-27 13：53

## 介绍

`add-apt-repository`命令的作用是添加PPA（PPA 全称 Personal Package Archive => 个人包归档）。

如果您对PPA还不够了解，请参考[PPA 完全指南](https://www.sysgeek.cn/ubuntu-ppa/)

## 需求分析

我们知道，在企业中经常有各种奇葩的需求。

比如说: 

>在CentOS上搭建apt源服务器

通常的做法是安装apt-mirror，然后在/etc/apt-mirror.list中添加镜像地址，然后运行apt-mirror命令，apt-mirror会根据apt-mirror.list中的信息，将远端镜像拉取到本地某个目录下。

存在以下几个问题：
1. 我们需要自己去寻找镜像地址
2. 找到地址后手动添加到apt-mirror.list

## 利用add-apt-repository
分析add-apt-repository后，发现它就是自动化做了类似于我们手动做的这样一件事情。

举个例子：
```shell
# 添加php => ppa
$ add-apt-repository ppa:ondrej/php -y 
```

## 源码分析
```python
/**
 *  Name: Main
 */
if __name__ == "__main__":
    usage = "usage: %prog [options] [repository]"
    parser = OptionParser(usage=usage)
    #add a dummy option which can be easily ignored
    parser.add_option("-?", dest="ignore", action="store_true", default=False)
    parser.add_option("-y", "--yes", dest="forceYes", action="store_true",
        help="force yes on all confirmation questions", default=False)
    parser.add_option("-r", "--remove", dest="remove", action="store_true",
        help="Remove the specified repository", default=False)

    # 1. 解析ppa:ondrej/php -y [-r]参数
    (options, args) = parser.parse_args()
    print("options:%s" % options)
    print("args:%s" % args)
    
    # 2. 在ubuntu系统上安装apt-get-repository后查看其源码找到该位置
    # version: tara
    config_dir = "/usr/share/mintsources/tara"   

    if not os.path.exists(config_dir):
        os.system("mkdir -p '%s'" % config_dir)

    ppa_head = args[0].split(":")[0]
    if len(args) > 1:
        sys.exit(1)
    ppa_line = args[0]
    
    # 3. 获取版本号`lsb_release -sc`
    lsb_codename = "tara"          
    config_parser = configparser.RawConfigParser()
    
    # 4. 读取config_dir中的XXXsources.conf配置信息
    config_parser.read("./mintsources.conf")     
    codename = "bionic"
    use_ppas = "true"
    if options.remove:
        # 删除镜像
        remove_repository_via_cli(ppa_line, codename, options.forceYes)
    else:
        
        # 添加镜像
        add_repository_via_cli(ppa_line, codename, options.forceYes, use_ppas)
    
/**
 *  Name: add_repository_via_cli
 *  @param: line
 *  @param: codename 
 *  @param: forceYes 
 *  @param: use_ppas 
 */
def add_repository_via_cli(line, codename, forceYes, use_ppas):
    if line.startswith("ppa:"):
        if use_ppas != "true":
            print(("Adding PPAs is not supported"))
            sys.exit(1)
        user, sep, ppa_name = line.split(":")[1].partition("/")
        ppa_name = ppa_name or "ppa"
        try:
            ppa_info = get_ppa_info_from_lp(user, ppa_name, codename)
        except Exception as detail:
            print (("Cannot add PPA: '%s'.") % detail)
            sys.exit(1)

        if "private" in ppa_info and ppa_info["private"]:
            print(("Adding private PPAs is not supported currently"))
            sys.exit(1)
        print(("You are about to add the following PPA:"))

        if ppa_info["description"] is not None:
            print(" %s" % (ppa_info["description"]))
        print((" More info: %s") % str(ppa_info["web_link"]))

        if sys.stdin.isatty():
            if not(forceYes):
                print(("Press Enter to continue or Ctrl+C to cancel"))
                sys.stdin.readline()
        else:
            if not(forceYes):
                print(("Unable to prompt for response.  Please run with -y"))
                sys.exit(1)

        (deb_line, file) = expand_ppa_line(line.strip(), codename)
        deb_line = expand_http_line(deb_line, codename)
        debsrc_line = 'deb-src' + deb_line[3:]

	# add the key
        short_key = ppa_info["signing_key_fingerprint"][-8:]
        os.system("gpg --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys %s" % short_key)

        # Add the PPA in sources.list.d
        with open(file, "a") as text_file:
            text_file.write("%s\n" % deb_line)
            text_file.write("%s\n" % debsrc_line)
    elif line.startswith("deb "):
        with open("/etc/apt-mirror.list", "a") as text_file:
            text_file.write("%s\n" % expand_http_line(line, codename))
            
/**
 *  Name: remove_repository_via_cli
 *  @param: line
 *  @param: codename 
 *  @param: forceYes 
 */
def remove_repository_via_cli(line, codename, forceYes):
    if line.startswith("ppa:"):
        user, sep, ppa_name = line.split(":")[1].partition("/")
        ppa_name = ppa_name or "ppa"
        try:
            ppa_info = get_ppa_info_from_lp(user, ppa_name, codename)
            print(("You are about to remove the following PPA:"))
            if ppa_info["description"] is not None:
                print(" %s" % (ppa_info["description"]))
            print((" More info: %s") % str(ppa_info["web_link"]))

            if sys.stdin.isatty():
                if not(forceYes):
                    print(("Press Enter to continue or Ctrl+C to cancel"))
                    sys.stdin.readline()
            else:
                if not(forceYes):
                        print(("Unable to prompt for response.  Please run with -y"))
                        sys.exit(1)

        except KeyboardInterrupt as detail:
            print (("Cancelling..."))
            sys.exit(1)
        except Exception as detail:
            print (("Cannot get info about PPA: '%s'.") % detail)

        (deb_line, file) = expand_ppa_line(line.strip(), codename)
        deb_line = expand_http_line(deb_line, codename)
        debsrc_line = 'deb-src' + deb_line[3:]

        # Remove the PPA from sources.list.d
        try:
            readfile = open(file, "r")
            content = readfile.read()
            readfile.close()
            content = content.replace(deb_line, "")
            content = content.replace(debsrc_line, "")
            with open(file, "w") as writefile:
                writefile.write(content)

          #  # If file no longer contains any "deb" instances, delete it as well
          #  if "deb " not in content:
          #      os.unlink(file)
        except IOError as detail:
            print (("failed to remove PPA: '%s'") % detail)

    elif line.startswith("deb "):
        # Remove the repository from sources.list.d
        file="/etc/apt-mirror.list"
        try:
            readfile = open(file, "r")
            content = readfile.read()
            readfile.close()
            content = content.replace(expand_http_line(line, codename), "")
            with open(file, "w") as writefile:
                writefile.write(content)

        except IOError as detail:
            print (("failed to remove repository: '%s'") % detail)
            
/**
 *  Name: get_ppa_info_from_lp
 *  @param: owner_name
 *  @param: ppa_name 
 *  @param: base_codename 
 */
def get_ppa_info_from_lp(owner_name, ppa_name, base_codename):
    DEFAULT_KEYSERVER = "hkp://keyserver.ubuntu.com:80/"
    # maintained until 2015
    LAUNCHPAD_PPA_API = 'https://launchpad.net/api/1.0/~%s/+archive/%s'
    # Specify to use the system default SSL store; change to a different path
    # to test with custom certificates.
    LAUNCHPAD_PPA_CERT = "/etc/ssl/certs/ca-certificates.crt"

    lp_url = LAUNCHPAD_PPA_API % (owner_name, ppa_name)
    try:
        json_data = requests.get(lp_url).json()
    except pycurl.error as e:
        raise PPAException("Error reading %s: %s" % (lp_url, e), e)

    # Make sure the PPA supports our base release
    repo_url = "http://ppa.launchpad.net/%s/%s/ubuntu/dists/%s" % (owner_name, ppa_name, base_codename)
    try:
        if (urlopen(repo_url).getcode() == 404):
            raise PPAException(("This PPA does not support %s") % base_codename)
    except Exception as e:
        print (e)
        raise PPAException(("This PPA does not support %s") % base_codename)
    return json_data
    
```

## 过程分析
上一小节中的源码并不是“真正”的apt-get-repository源码，它是我自己根据自己的Ubuntu系统信息修改的，以使其跑在CentOS机器上。

其实对于mint（这是我使用的Ubuntu的一个版本linuxMint系统）来说，add-apt-repository执行后，会去调用`/usr/lib/linuxmint/mintSources/mintSources.py`这个python脚本去真正的执行添加apt软件源操作，然后根据ppa信息创建一个`XXX.list`文件置于`/etc/apt/sources.list.d/`目录下，并将获取到的软件源镜像地址添加到XXX.list文件中。这样在`apt-get update`后就会更新apt软件源地址，就可以愉快的安装添加的软件源中的软件了。

所以，你要根据你自己的ubuntu系统找到add-apt-repository源码，对其加以修改。代码很长，大约有1600多行。但是核心的部分也就是上面三个主要的函数`add_apt_repository_via_cli`,`remove_apt_repository_via_cli`,`get_ppa_info_from_lp`。

代码相信大家有点基础的都能看的懂，所以这里我就讲一下核心思想：
1. `add-apt-repository ppa:ondrej/php -y`运行后，首先解析命令行参数`ppa:ondrej/php -y -r =>  {'ppa','ondrej','php'}  {'-y','-r'}` 
2. 获取Ubuntu系统的版本代号`lsb_release -sc`以及读取系统配置文件XXXsources.conf   # 根据主机信息获取PPA平台对应版本的镜像
3. 通过`add_repository_via_cli()`函数将PPA软件源镜像地址添加到/etc/apt-mirror.list
4. 通过`remove_repository_via_cli()`函数将PPA软件源镜像地址从/etc/apt-mirror.list删除
5. 通过`get_ppa_info_from_lp()`函数获取Launchpad平台存储库镜像地址

你可以对照我提供的模板，然后再根据你自己机器上的add-apt-repository相关源码进行修改。

最后，明确一下，安装apt-mirror是前提，根据你自己的需求去修改源代码。

[add-apt-getrepository 添加软件源模板 (ubuntu/centos都试用)]()

>版权声明：©2019 Jun 转载请保留原文链接及作者