#!/data/anaconda3/bin/python

# -----------------------------------
#
# @FileName: add-apt-repository
#
# @Author: Cai Jun
#
# @Email: JohnCai.caijun.cn@gmail.com
#
# @Date: 2019-07-27 13:29
#
# -----------------------------------

import os
import configparser
import requests
import pycurl
import sys
import re
from urllib.request import urlopen
from optparse import OptionParser

def expand_http_line(line, distro_codename):
    """
    short cut - this:
      apt-add-repository http://packages.medibuntu.org free non-free
    same as
      apt-add-repository 'deb http://packages.medibuntu.org/ base_codename free non-free'
    """
    if not line.startswith("http"):
      return line
    repo = line.split()[0]
    try:
        areas = line.split(" ",1)[1]
    except IndexError:
        areas = "main"
    line = "deb %s %s %s" % ( repo, distro_codename, areas )
    return line

def encode(s):
    return re.sub("[^a-zA-Z0-9_-]", "_", s)

def expand_ppa_line(abrev, distro_codename):
    # leave non-ppa: lines unchanged
    if not abrev.startswith("ppa:"):
        return (abrev, None)
    # FIXME: add support for dependency PPAs too (once we can get them
    #        via some sort of API, see LP #385129)
    abrev = abrev.split(":")[1]
    ppa_owner = abrev.split("/")[0]
    try:
        ppa_name = abrev.split("/")[1]
    except IndexError as e:
        ppa_name = "ppa"
    line = "deb http://ppa.launchpad.net/%s/%s/ubuntu %s main" % (ppa_owner, ppa_name, distro_codename)
    filename = "/etc/apt-mirror.list"
    return (line, filename)

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

if __name__ == "__main__":
    usage = "usage: %prog [options] [repository]"
    parser = OptionParser(usage=usage)
    #add a dummy option which can be easily ignored
    parser.add_option("-?", dest="ignore", action="store_true", default=False)
    parser.add_option("-y", "--yes", dest="forceYes", action="store_true",
        help="force yes on all confirmation questions", default=False)
    parser.add_option("-r", "--remove", dest="remove", action="store_true",
        help="Remove the specified repository", default=False)

    (options, args) = parser.parse_args()
    print("options:%s" % options)
    print("args:%s" % args)

    # version: tara
    config_dir = "/usr/share/mintsources/tara"

    if not os.path.exists(config_dir):
        os.system("mkdir -p '%s'" % config_dir)

    ppa_head = args[0].split(":")[0]
    if len(args) > 1:
        sys.exit(1)
    ppa_line = args[0]
    lsb_codename = "tara"
    config_parser = configparser.RawConfigParser()
    config_parser.read("./mintsources.conf")
    codename = "bionic"
    use_ppas = "true"
    if options.remove:
        remove_repository_via_cli(ppa_line, codename, options.forceYes)
    else:
        add_repository_via_cli(ppa_line, codename, options.forceYes, use_ppas)
