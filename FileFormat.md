# Database file format

Numbers in the file should be little endian. (Through the code do not show this.)

## Pages

Divide the file into pages by 8KB. The first page of the file is database metadata page. Other pages can be table metadata page, data page, index page, and empty page.

## Database metadata page

- 0-3: Magic value "DTBS"
- 4-251: Reserved
- 252-255: Number of tables
- 256-4095: 30 x 128 byte table data, for every table:
	- 0-3: Page ID of table metadata page
	- 4-128: Table name (zero terminated)
- 4096-8191：Bitset that indicates whether a page is used (8*4096=32768 pages)

## Table metadata page

- 0-243: Reserved
- 244-247: Page ID of first data page of the table
- 248-251: Length of records
- 252-255: Number of records
- 256-8191: Reserved

## Data page

- 0-3: Page ID of previous data page of the table
- 4-7: Page ID of next data page of the table
- 8-87: Reserved
- 88-91: Length of records
- 92-95: Number of records
- 96-8191: Records:
	- 96-?: Real Record data
	- ?-8191: Bitset that indicates whether a record field is used

## Index page

- 0-8191: Reserved

## Empty page

It is literally empty.