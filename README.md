# my research in sdj1
Secret Sharing Command based Shamir's Secret Sharing Scheme

## Usage
Usage is the below, coming update soon...

### Mode
 - split (implemented)
	```
	./shamirss8bit --mode=split -n num -k num [FILE]
	-n num > -k num
	```

 - combine (implemented)
	```
	./shamirss8bit --mode=combine [FILE ...]
	```

## Description
 - Managing risk of information disclosure and losing (and more) for your secret file.
 - In split step
	 - read a byte at first, then it is processed by this program that operation on GF(2^8), and generate pieces of information depending on parameter n called "share".
	 - created file and written share, and you store the share in local system or server or more.
 - In combine step
	 - you gather shares the number of k and execute command like above.
	 - created file "secret.reconst" is your secret text.

## Future
 - ~Add options that the number of shares and threshold~ (implemented)
 - Secure coding like memory management, and more
 - Able to process image file, pdf and more(now, this program can process only text file).
