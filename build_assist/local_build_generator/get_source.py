#!/usr/bin/env python3
from __future__ import print_function

import json, argparse, os, shutil, sys
import subprocess as sp
import multiprocessing as mp

class DirectoryStack:
  def __init__( self, stdout = sys.stdout ):
    self.stack = []
    self.stdout = stdout

  def pushd( self, path ):
    # Store pwd, then cd to path
    self.stack.append( os.getcwd() )
    os.chdir( path )
    self.print_stack()


  def popd( self ):
    if len( self.stack ) > 0:
      # cd to first path on stack
      os.chdir( self.stack.pop() )
      self.print_stack()
    else:
      self.stdout.write( "popd: Directory stack empty\n" );

  def print_stack( self ):
    self.stdout.write( " ".join( self.stack + [os.getcwd()] ) + "\n" )


def downloader_task( work ):
  return downloader_action( **work )


def git_method( url, root, destination, tag=None ):
  print( "Cloning", url )
  dir_stack = DirectoryStack(  )

  clone_location = "{}/{}".format( root, destination )

  process = sp.Popen( ['git', 'clone', url, clone_location ], stdout=sp.PIPE, stderr=sp.PIPE )
  stdout, stderr = process.communicate()
  status = process.wait()
  assert status == 0, "Could not clone {} to {}.\nExit code {}\nMessage: {}".format(url, destination, status, stderr)

  if tag != None:
    dir_stack.pushd( clone_location )

    process = sp.Popen( ['git', 'checkout', 'tags/{}'.format(tag), '-b', 'tag_{}'.format(tag)], stdout=sp.PIPE, stderr=sp.PIPE )
    stdout, stderr = process.communicate()
    status = process.wait()

    dir_stack.popd()

    assert status == 0, "Could not checkout tag {} from {}.\nExit code {}\nMessage: {}".format(tag, url, status, stderr)


def wget_method( url, root, destination, archive_info ):
  print( "Downloading", url )

  dir_stack = DirectoryStack()
  dir_stack.pushd( root )

  process = sp.Popen( ['wget', url], stdout=sp.PIPE, stderr=sp.PIPE )
  stdout, stderr = process.communicate()
  status = process.wait()
  assert status == 0, "Could not download {}.\nExit code {}\nMessage: {}".format( url, status, stderr )

  extract_archive( archive_info["name"], archive_info["type"], archive_info["root_dir"], destination )

  os.remove( archive_info["name"] )

  dir_stack.popd()


def extract_archive( archive_name, archive_type, root, destination ):

  if archive_type == "zip":
    command = [ "unzip", archive_name ]
  elif archive_type == "tar":
    command = [ "tar", "xvf", archive_name ]
  else:
    raise Exception( "Unknown archive type: {}".format( archive_type ) )

  process = sp.Popen( command, stdout=sp.PIPE, stderr=sp.PIPE )
  stdout, stderr = process.communicate()
  status = process.wait()

  assert status == 0, "Could not extract \"{}\"\nExit code {}\nMessage: {}".format( archive_name, status, stderr )

  os.rename( root, destination )

def downloader_action( package, args ):

  resulting_path = "{}/{}".format( args["destination"], package["name"] )

  if os.path.exists( resulting_path ):
    shutil.rmtree( resulting_path )

  if package["method"] == "git":
    git_method( package["url"], args["destination"], package["name"], (package["tag"] if "tag" in package else None) )
  elif package["method"] == "wget":
    wget_method( package["url"], args["destination"], package["name"], package["archive_info"] )
  else:
    raise Exception( "Unknown download method: {}".format( package["method"] ) )


def main():
  argparser = argparse.ArgumentParser( description="Sources downloader" )
  argparser.add_argument( "-s", "--source_json", type=str, default="sources.json" )
  argparser.add_argument( "-p", "--processes", type=int, default=int( max( 1, mp.cpu_count() / 2 ) ) )
  argparser.add_argument( "-d", "--destination", type=str, default="./local_build/source" )
  argparser.add_argument( "-o", "--only-these-packages", type=str, nargs="+", default=[] )
  argparser.add_argument( "-l", "--list-packages", const=True, action="store_const", default=False )
  args = argparser.parse_args()

  with open( args.source_json ) as source_file:
    packages =  json.load( source_file )

  if args.list_packages:
    print( "Package list:", *list(map( lambda p: "  + " + p["name"], packages ) ), sep="\n" )
    return 0

  if len( args.only_these_packages ) > 0:
    non_existing = set(args.only_these_packages) - set( map( lambda p: p["name"], packages ) )
    if len( non_existing ) > 0:
      raise Exception( "The specified package(s) do not exist: {}".format( ", ".join( non_existing ) ) )
    packages = filter(
      lambda package: package["name"] in args.only_these_packages,
      packages
    )

  pool = mp.Pool( args.processes )
  pool.map(
    downloader_task,
    map(
      lambda package: {
        "package" : package,
        "args" : vars( args )
      },
      packages
    )
  )

if __name__ == "__main__":
  main()
