#include "filesystem_fake.h"
#include "request_handler_crud.h"
#include "gtest/gtest.h"
#include <boost/beast.hpp>
#include <boost/system/error_code.hpp>
#include <memory>

class CrudHandlerTest : public ::testing::Test
{
  protected:
    std::string location = "/api";
    std::string data_path = "./crud";
    std::unique_ptr<FileSystem> fs;

    http::request<http::string_body> req;
    http::response<http::string_body> res;

    virtual void SetUp()
    {
        fs = std::make_unique<FakeFileSystem>();
    }

    virtual void TearDown()
    {
        req.clear();
        res.clear();
    }

    void verify_entity(CrudHandler &handler, std::string target, std::string expected_body)
    {
        req.clear();
        res.clear();

        // Verify that entity is retrievable and has the expected body
        req.method(http::verb::get);
        req.target(target);
        req.version(11);
        req.body() = "";
        req.prepare_payload();

        handler.handle_request(req, res);

        EXPECT_EQ(res.result(), http::status::ok);
        EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
        EXPECT_EQ(res.body(), expected_body);
    }

    void verify_missing_entity(CrudHandler &handler, std::string target)
    {
        req.clear();
        res.clear();

        // Verify that entity is not retrievable / does not exist
        req.method(http::verb::get);
        req.target(target);
        req.version(11);
        req.body() = "";
        req.prepare_payload();

        handler.handle_request(req, res);

        EXPECT_EQ(res.result(), http::status::not_found);
    }
};

/*
 * Tests for Retrieve action (handle_get())
 */
TEST_F(CrudHandlerTest, RetrieveEntity)
{
    std::string entity_body = "{ \"foo\": \"bar\" }";
    fs->write(data_path + "/Thing/0", entity_body);

    std::string target = location + "/Thing/0";
    req.method(http::verb::get);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::ok);
    EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
    EXPECT_EQ(res.body(), entity_body);
}

TEST_F(CrudHandlerTest, RetrieveMissingEntity)
{
    std::string target = location + "/Thing/0";
    req.method(http::verb::get);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(CrudHandlerTest, RetrieveEntityWithBadId)
{
    std::string entity_body = "{ \"foo\": \"bar\" }";
    fs->write(data_path + "/Thing/invalid_id", entity_body);

    std::string target = location + "/Thing/invalid_id";
    req.method(http::verb::get);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

/*
 * Tests for List action (handle_get())
 */
TEST_F(CrudHandlerTest, ListEntityType)
{
    fs->write(data_path + "/Thing/0", "{ \"foo\": \"bar\" }");
    fs->write(data_path + "/Thing/2", "{ \"lorem\": \"ipsum\" }");

    std::string target = location + "/Thing";
    req.method(http::verb::get);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::ok);
    EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
    EXPECT_EQ(res.body(), "[0, 2]");
}

TEST_F(CrudHandlerTest, ListEmptyEntityType)
{
    fs->write(data_path + "/Thing/_", "");

    std::string target = location + "/Thing";
    req.method(http::verb::get);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::ok);
    EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
    EXPECT_EQ(res.body(), "[]");
}

TEST_F(CrudHandlerTest, ListMissingEntityType)
{
    std::string target = location + "/Thing";
    req.method(http::verb::get);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(CrudHandlerTest, ListIgnoreBadIds)
{
    fs->write(data_path + "/Thing/0", "{ \"foo\": \"bar\" }");
    fs->write(data_path + "/Thing/1a", "{ \"lorem\": \"ipsum\" }");
    fs->write(data_path + "/Thing/2", "{ \"dolor\": \"sit\" }");
    fs->write(data_path + "/Thing/cheese", "{ \"amet\": \"consectetur\" }");

    std::string target = location + "/Thing";
    req.method(http::verb::get);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::ok);
    EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
    EXPECT_EQ(res.body(), "[0, 2]");
}

/*
 * Tests for Create action (handle_post())
 */
TEST_F(CrudHandlerTest, CreateEntity)
{
    fs->write(data_path + "/Thing/0", "{ \"foo\": \"bar\" }");

    std::string target = location + "/Thing";
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    req.method(http::verb::post);
    req.target(target);
    req.version(11);
    req.body() = entity_body;
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::created);
    EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
    EXPECT_EQ(res.body(), "{\"id\": 1}");
}

TEST_F(CrudHandlerTest, CreateEntityOfNewType)
{
    std::string target = location + "/Thing";
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    req.method(http::verb::post);
    req.target(target);
    req.version(11);
    req.body() = entity_body;
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::created);
    EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
    EXPECT_EQ(res.body(), "{\"id\": 0}");
}

TEST_F(CrudHandlerTest, CreateFirstEntity)
{
    fs->write(data_path + "/Thing/_", "");

    std::string target = location + "/Thing";
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    req.method(http::verb::post);
    req.target(target);
    req.version(11);
    req.body() = entity_body;
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::created);
    EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
    EXPECT_EQ(res.body(), "{\"id\": 0}");
}

TEST_F(CrudHandlerTest, CreateMiddleEntity)
{
    fs->write(data_path + "/Thing/0", "{ \"foo\": \"bar\" }");
    fs->write(data_path + "/Thing/1cheese", "");
    fs->write(data_path + "/Thing/2", "{ \"lorem\": \"ipsum\" }");

    std::string target = location + "/Thing";
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    req.method(http::verb::post);
    req.target(target);
    req.version(11);
    req.body() = entity_body;
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::created);
    EXPECT_EQ(res.base().at(http::field::content_type), "application/json");
    EXPECT_EQ(res.body(), "{\"id\": 1}");
}

TEST_F(CrudHandlerTest, CreateInvalidTarget)
{
    fs->write(data_path + "/Thing/foo", "");

    std::string target = location + "/Thing/foo";
    req.method(http::verb::post);
    req.target(target);
    req.version(11);
    req.body() = "{ \"lorem\": \"ipsum\" }";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

/*
 * Tests for Update action (handle_put())
 */
TEST_F(CrudHandlerTest, UpdateEntity)
{
    fs->write(data_path + "/Thing/0", "{ \"foo\": \"bar\" }");

    std::string target = location + "/Thing/0";
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    req.method(http::verb::put);
    req.target(target);
    req.version(11);
    req.body() = entity_body;
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::ok);
    verify_entity(handler, target, entity_body);
}

TEST_F(CrudHandlerTest, UpdateNewEntity)
{
    fs->write(data_path + "/Thing/0", "{ \"foo\": \"bar\" }");

    std::string target = location + "/Thing/2";
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    req.method(http::verb::put);
    req.target(target);
    req.version(11);
    req.body() = entity_body;
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::created);
    verify_entity(handler, target, entity_body);
}

TEST_F(CrudHandlerTest, UpdateEntityOfNewType)
{
    std::string target = location + "/Thing/3";
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    req.method(http::verb::put);
    req.target(target);
    req.version(11);
    req.body() = entity_body;
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::created);
    verify_entity(handler, target, entity_body);
}

TEST_F(CrudHandlerTest, UpdateInvalidTarget)
{
    fs->write(data_path + "/Thing/1/foo", "");

    std::string target = location + "/Thing/1";
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    req.method(http::verb::put);
    req.target(target);
    req.version(11);
    req.body() = entity_body;
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

/*
 * Tests for Delete action (handle_delete())
 */
TEST_F(CrudHandlerTest, DeleteEntity)
{
    std::string entity_body = "{ \"foo\": \"bar\" }";
    fs->write(data_path + "/Thing/0", entity_body);

    std::string target = location + "/Thing/0";
    req.method(http::verb::delete_);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::ok);
    verify_missing_entity(handler, target);
}

TEST_F(CrudHandlerTest, DeleteEntityWithBadId)
{
    std::string entity_body = "{ \"foo\": \"bar\" }";
    fs->write(data_path + "/Thing/invalid_id", entity_body);

    std::string target = location + "/Thing/invalid_id";
    req.method(http::verb::delete_);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

TEST_F(CrudHandlerTest, DeleteMissingEntity)
{
    std::string target = location + "/Thing/0";
    req.method(http::verb::delete_);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(CrudHandlerTest, DeleteInvalidTarget)
{
    std::string entity_body = "{ \"lorem\": \"ipsum\" }";
    fs->write(data_path + "/Thing/1/foo", entity_body);

    std::string target = location + "/Thing/1";
    req.method(http::verb::delete_);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

/*
 * Miscellaneous tests
 */
TEST_F(CrudHandlerTest, NotImplementedMethod)
{
    std::string target = location + "/Thing/1";
    req.method(http::verb::patch);
    req.target(target);
    req.version(11);
    req.body() = "";
    req.prepare_payload();

    CrudHandler handler(location, target, data_path, std::move(fs));
    handler.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::not_implemented);
}