#include "limitless/catch_amalgamated.hpp"

#include <serialization/uniform_serializer.hpp>
#include <core/uniform.hpp>
#include <util/bytebuffer.hpp>

using namespace Limitless;

TEST_CASE("serialize UniformValue") {
    UniformSerializer serializer;

    UniformValue<float> uniform{"test_uniform", 13.5f};

    auto buffer = serializer.serialize(uniform);

    auto deserialized_uniform = serializer.deserialize(buffer);

    REQUIRE(deserialized_uniform->getName() == "test_uniform");
    REQUIRE_NOTHROW(dynamic_cast<UniformValue<float>&>(*deserialized_uniform));
    REQUIRE(deserialized_uniform->getValueType() == UniformValueType::Float);
    REQUIRE(dynamic_cast<UniformValue<float>&>(*deserialized_uniform).getValue() == 13.5f);
}