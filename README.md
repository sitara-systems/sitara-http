# ofxCurl

A wrapper to simplify using the libcurl for transferring data, typically over a network.  This implementation uses a stateful curl instance, so that you don't need to reset options after every call.

## Requirements
None.

### Windows
To use this add-on, use the projector generator.  Once you've created a project, use the `ofxCurl.props` property sheet in `config/` to set up your compiler properly.

This library has been tested with Visual Studio 2015 and openFrameworks v0.9.1

### OSX
This project is currently not configured for Xcode in OSX.  If anyone wants to take this on, I'd be glad to help.  Otherwise you'll have to wait until I have a reason to need it on my Mac :)

## Usage
`ofxCurl` is a a relatively minimal wrapper around the libcurl library.

## Examples

## Contributing
This project uses the [Git Flow](http://nvie.com/posts/a-successful-git-branching-model/) paradigm.  Before contributing, please make your own feature branch with your changes.

## More Information
* [libcurl Homepage](https://curl.haxx.se/)