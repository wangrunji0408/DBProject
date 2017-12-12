//
// Created by 王润基 on 2017/11/20.
//

#include <gtest/gtest.h>
#include <system/DatabaseManager.h>
#include <system/TableMetaPage.h>
#include "../TestBase.h"

namespace {

class TestTableDef : public TestBase
{
protected:
	void SetUp() override {
		TestBase::SetUp();
	}

	void Reopen () override {
		TestBase::Reopen();
	}
};

TEST_F(TestTableDef, SizeofTableMetaPage)
{
	ASSERT_EQ(8192, sizeof(TableMetaPage));
	ASSERT_EQ(64, sizeof(TableMetaPage::Column));
	TableMetaPage* meta = nullptr;
	ASSERT_EQ(248, (char*)&meta->firstPageID - (char*)meta);
	ASSERT_EQ(252, (char*)&meta->recordLength - (char*)meta);
}

TEST_F(TestTableDef, CanCreateTableFromDefAndGetDef)
{
	/*
		CREATE TABLE customer(
			id INT(10) NOT NULL,
			name VARCHAR(25) NOT NULL,
			gender VARCHAR(1) NOT NULL,
			PRIMARY KEY (id)
		);
	*/
	auto customer = TableDef();
	customer.name = "customer";
	customer.columns.push_back(ColumnDef{"id", INT, 4, false, false});
	customer.columns.push_back(ColumnDef{"name", VARCHAR, 25, false, false});
	customer.columns.push_back(ColumnDef{"gender", VARCHAR, 1, false, false});
	customer.primaryKeys.emplace_back("id");
	db->createTable(customer);

	/*
		CREATE TABLE book (
			id INT(10) NOT NULL,
			title VARCHAR(100) NOT NULL,
			authors VARCHAR(200),
			publisher VARCHAR(100),
			copies INT(10),
			PRIMARY KEY (id)
		);
	*/
	auto book = TableDef();
	book.name = "book";
	book.columns.push_back(ColumnDef{"id", INT, 4, false, false});
	book.columns.push_back(ColumnDef{"title", VARCHAR, 100, false, false});
	book.columns.push_back(ColumnDef{"authors", VARCHAR, 200, true, false});
	book.columns.push_back(ColumnDef{"publisher", VARCHAR, 100, true, false});
	book.columns.push_back(ColumnDef{"copies", INT, 4, true, false});
	book.primaryKeys.emplace_back("id");
	db->createTable(book);

	/*
		CREATE TABLE website (
			id INT(10) NOT NULL,
			name VARCHAR(25) NOT NULL,
			url VARCHAR(100),
			PRIMARY KEY (id)
		);
	*/
	auto website = TableDef();
	website.name = "website";
	website.columns.push_back(ColumnDef{"id", INT, 4, false, false});
	website.columns.push_back(ColumnDef{"name", VARCHAR, 25, false, false});
	website.columns.push_back(ColumnDef{"url", VARCHAR, 100, true, false});
	website.primaryKeys.emplace_back("id");
	db->createTable(website);

	/*
		CREATE TABLE price (
			website_id INT(10) NOT NULL,
			book_id INT(10) NOT NULL,
			price FLOAT NOT NULL,
			PRIMARY KEY (website_id,book_id),
			FOREIGN KEY (website_id) REFERENCES website(id),
			FOREIGN KEY (book_id) REFERENCES book(id)
		);
	*/
	auto price = TableDef();
	price.name = "price";
	price.columns.push_back(ColumnDef{"website_id", INT, 4, false, false});
	price.columns.push_back(ColumnDef{"book_id", INT, 4, false, false});
	price.columns.push_back(ColumnDef{"price", FLOAT, 4, false, false});
	price.primaryKeys.emplace_back("website_id");
	price.primaryKeys.emplace_back("book_id");
	price.foreignKeys.push_back(ForeignKeyDef{"website_id", "website", "id"});
	price.foreignKeys.push_back(ForeignKeyDef{"book_id", "book", "id"});
	db->createTable(price);

	/*
		CREATE TABLE orders (
			id INT(10) NOT NULL,
			website_id INT(10) NOT NULL,
			customer_id INT(10) NOT NULL,
			book_id INT(10) NOT NULL,
			date DATE,
			quantity INT(10),
			PRIMARY KEY (id),
			FOREIGN KEY (website_id) REFERENCES website(id),
			FOREIGN KEY (customer_id) REFERENCES customer(id),
			FOREIGN KEY (book_id) REFERENCES book(id)
		);
	*/
	auto orders = TableDef();
	orders.name = "orders";
	orders.columns.push_back(ColumnDef{"id", INT, 4, false, false});
	orders.columns.push_back(ColumnDef{"website_id", INT, 4, false, false});
	orders.columns.push_back(ColumnDef{"customer_id", INT, 4, false, false});
	orders.columns.push_back(ColumnDef{"book_id", INT, 4, false, false});
	orders.columns.push_back(ColumnDef{"date", DATE, 4, false, false});
	orders.columns.push_back(ColumnDef{"quantity", INT, 4, false, false});
	orders.primaryKeys.emplace_back("id");
	orders.foreignKeys.push_back(ForeignKeyDef{"website_id", "website", "id"});
	orders.foreignKeys.push_back(ForeignKeyDef{"customer_id", "customer", "id"});
	orders.foreignKeys.push_back(ForeignKeyDef{"book_id", "book", "id"});
	db->createTable(orders);

	Reopen();
	ASSERT_EQ(customer, db->getTableDef("customer"));
	ASSERT_EQ(book, db->getTableDef("book"));
	ASSERT_EQ(website, db->getTableDef("website"));
	ASSERT_EQ(price, db->getTableDef("price"));
	ASSERT_EQ(orders, db->getTableDef("orders"));
}

}