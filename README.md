# Simple-Config-Parser
Simple Config Parser (SCP) - is a library for a simple, fast, zero-copy and flexible parsing of scpl-format in C-lanugage.

SCP syntax example:
```scp
theme = [
	number = 4,
	string = 'foo',
	arrays = [
		5,
		'string'
	],
	tables = [
		something = 'a'
	]
	// Comment
	/* Comment Block */
]
```
