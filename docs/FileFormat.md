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
- 4096-8191：Bitset that indicates whether a page is used (8*4096=32768 pages) (from left to right)

## Table metadata page

- 0-247: Reserved
- 248-251: Page ID of first data page of the table (-1 if there is no data page)
- 252-255: Length of records
- 256-8191: Reserved

## Data page

- 0-3: Page ID of previous data page of the table (-1 if this is first data page)
- 4-7: Page ID of next data page of the table (-1 if this is last data page)
- 8-11: Page ID of the table metadata page
- 12-87: Reserved
- 88-91: Length of records
- 92-95: Number of records
- 96-8191: Records:
	- 96-?: Real Record data
	- ?-8191: Bitset that indicates whether a record field is used (from right to left)

## SysIndex page

- 0-95: Not sure. See `SysIndexPage` class for details.
- 96-8191: 506 x 16 byte index data, for every index:
    - 0-3: Index root page ID
    - 4-7: Table meta page ID
    - 8-9: Key type
    - 10-11: Key length
    - 12-15: Reserved

## Index page

- 0-95: Not sure. See `IndexPage` class for details.
- 96-8191: Index records: s0 s1 ... s\[n-1]
    - Max(n) = floor(8096 / slotSize)
    - Slot
        - Not Leaf: key? RID4 pageID2
        - Leaf:     key? RID4

## Empty page

It is literally empty.