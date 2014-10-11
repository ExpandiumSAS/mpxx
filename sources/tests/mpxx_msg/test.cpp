#define BOOST_TEST_MODULE mpxx_msg

#include <string>
#include <sstream>

#include <mpxx/unit_test.hpp>
#include <mpxx/mpxx.hpp>

struct msg_id {};
struct msg_str {};

typedef mpxx::msg<
    mpxx::tag<msg_id, int>,
    mpxx::tag<msg_str, std::string>
> msg_type;

BOOST_AUTO_TEST_CASE(mpxx_msg)
{
    msg_type m(42, "a message");

    BOOST_CHECK_MESSAGE(
        m.get<0>() == 42 && m.get<1>() == "a message",
        "access values by index"
    );

    BOOST_CHECK_MESSAGE(
        m.get<msg_id>() == 42 && m.get<msg_str>() == "a message",
        "access values by tag"
    );
}

BOOST_AUTO_TEST_CASE(mpxx_msg_pack_unpack)
{
    msg_type m(42, "a message");

    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, m);

    msgpack::unpacked u;
    msgpack::unpack(&u, sbuf.data(), sbuf.size());

    msg_type rm;
    msgpack::object obj = u.get();
    obj.convert(&rm);

    BOOST_CHECK_MESSAGE(
        rm.get<0>() == 42 && rm.get<1>() == "a message",
        "access values by index"
    );

    BOOST_CHECK_MESSAGE(
        rm.get<msg_id>() == 42 && rm.get<msg_str>() == "a message",
        "access values by tag"
    );
}

BOOST_AUTO_TEST_CASE(mpxx_msg_dump)
{
    msg_type m(42, "a message");

    std::ostringstream oss;
    oss << m;

    BOOST_CHECK_MESSAGE(
        oss.str() == "42,a message",
        "dump to stream"
    );
}

BOOST_AUTO_TEST_CASE(mpxx_msg_access)
{
    msg_type m(42, "a message");

    m.set<0, 1>(44, "junk");

    BOOST_CHECK_MESSAGE(
        m[msg_id()] == 44 && m[msg_str()] == "junk",
        "compound assignment via set<>"
    );

    m[msg_id()] = 43;

    BOOST_CHECK_MESSAGE(
        m[msg_id()] == 43,
        "assignment via operator[]"
    );

    m.set<1>("another message");

    BOOST_CHECK_MESSAGE(
        m[msg_str()] == "another message",
        "assignment via set<>"
    );

    auto t = m(msg_id(), msg_str());

    BOOST_CHECK_MESSAGE(
        std::get<0>(t) == 43 && std::get<1>(t) == "another message",
        "message value tuple"
    );

    m(msg_id(), msg_str()) = std::make_tuple(1234, "whoa");

    BOOST_CHECK_MESSAGE(
        m[msg_id()] == 1234 && m[msg_str()] == "whoa",
        "compound assignment via operator[]"
    );

    BOOST_CHECK_MESSAGE(
        std::get<0>(t) == 1234 && std::get<1>(t) == "whoa",
        "message value tuple updated"
    );
}