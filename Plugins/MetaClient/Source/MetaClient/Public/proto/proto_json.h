//------------------------------------------------------------------------------
// Automatically generated by the Fast Binary Encoding compiler, do not modify!
// https://github.com/chronoxor/FastBinaryEncoding
// Source: meta.lib.proto.fbe
// FBE version: 1.14.2.0
//------------------------------------------------------------------------------

#pragma once

#if defined(__clang__)
#pragma clang system_header
#elif defined(__GNUC__)
#pragma GCC system_header
#elif defined(_MSC_VER)
#pragma system_header
#endif

#include "fbe_json.h"

#include "proto.h"

namespace FBE {

namespace JSON {

template <class TWriter>
struct ValueWriter<TWriter, ::proto::MetaPacket>
{
    static bool to_json(TWriter& writer, const ::proto::MetaPacket& value, bool scope = true)
    {
        if (scope)
            if (!writer.StartObject())
                return false;
        if (!FBE::JSON::to_json_key(writer, "type") || !FBE::JSON::to_json(writer, value.type, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "number") || !FBE::JSON::to_json(writer, value.number, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "text") || !FBE::JSON::to_json(writer, value.text, true))
            return false;
        if (scope)
            if (!writer.EndObject())
                return false;
        return true;
    }
};

template <class TJson>
struct ValueReader<TJson, ::proto::MetaPacket>
{
    static bool from_json(const TJson& json, ::proto::MetaPacket& value, const char* key = nullptr)
    {
        if (key != nullptr)
            return FBE::JSON::from_json_child(json, value, key);
        bool result = true;
        result &= FBE::JSON::from_json(json, value.type, "type");
        result &= FBE::JSON::from_json(json, value.number, "number");
        result &= FBE::JSON::from_json(json, value.text, "text");
        return result;
    }
};

template <class TWriter>
struct ValueWriter<TWriter, ::proto::MetaBinaryPacket>
{
    static bool to_json(TWriter& writer, const ::proto::MetaBinaryPacket& value, bool scope = true)
    {
        if (scope)
            if (!writer.StartObject())
                return false;
        if (!FBE::JSON::to_json_key(writer, "type") || !FBE::JSON::to_json(writer, value.type, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "data") || !FBE::JSON::to_json(writer, value.data, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "info") || !FBE::JSON::to_json(writer, value.info, true))
            return false;
        if (scope)
            if (!writer.EndObject())
                return false;
        return true;
    }
};

template <class TJson>
struct ValueReader<TJson, ::proto::MetaBinaryPacket>
{
    static bool from_json(const TJson& json, ::proto::MetaBinaryPacket& value, const char* key = nullptr)
    {
        if (key != nullptr)
            return FBE::JSON::from_json_child(json, value, key);
        bool result = true;
        result &= FBE::JSON::from_json(json, value.type, "type");
        result &= FBE::JSON::from_json(json, value.data, "data");
        result &= FBE::JSON::from_json(json, value.info, "info");
        return result;
    }
};

template <class TWriter>
struct ValueWriter<TWriter, ::proto::LiDARPosition>
{
    static bool to_json(TWriter& writer, const ::proto::LiDARPosition& value, bool scope = true)
    {
        if (scope)
            if (!writer.StartObject())
                return false;
        if (!FBE::JSON::to_json_key(writer, "idx") || !FBE::JSON::to_json(writer, value.idx, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "x") || !FBE::JSON::to_json(writer, value.x, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "y") || !FBE::JSON::to_json(writer, value.y, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "originPercentX") || !FBE::JSON::to_json(writer, value.originPercentX, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "originPercentY") || !FBE::JSON::to_json(writer, value.originPercentY, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "percentX") || !FBE::JSON::to_json(writer, value.percentX, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "percentY") || !FBE::JSON::to_json(writer, value.percentY, true))
            return false;
        if (scope)
            if (!writer.EndObject())
                return false;
        return true;
    }
};

template <class TJson>
struct ValueReader<TJson, ::proto::LiDARPosition>
{
    static bool from_json(const TJson& json, ::proto::LiDARPosition& value, const char* key = nullptr)
    {
        if (key != nullptr)
            return FBE::JSON::from_json_child(json, value, key);
        bool result = true;
        result &= FBE::JSON::from_json(json, value.idx, "idx");
        result &= FBE::JSON::from_json(json, value.x, "x");
        result &= FBE::JSON::from_json(json, value.y, "y");
        result &= FBE::JSON::from_json(json, value.originPercentX, "originPercentX");
        result &= FBE::JSON::from_json(json, value.originPercentY, "originPercentY");
        result &= FBE::JSON::from_json(json, value.percentX, "percentX");
        result &= FBE::JSON::from_json(json, value.percentY, "percentY");
        return result;
    }
};

template <class TWriter>
struct ValueWriter<TWriter, ::proto::LiDARFrameData>
{
    static bool to_json(TWriter& writer, const ::proto::LiDARFrameData& value, bool scope = true)
    {
        if (scope)
            if (!writer.StartObject())
                return false;
        if (!FBE::JSON::to_json_key(writer, "screen") || !FBE::JSON::to_json(writer, value.screen, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "newTouches") || !FBE::JSON::to_json(writer, value.newTouches, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "removeTouches") || !FBE::JSON::to_json(writer, value.removeTouches, true))
            return false;
        if (!FBE::JSON::to_json_key(writer, "updateTouches") || !FBE::JSON::to_json(writer, value.updateTouches, true))
            return false;
        if (scope)
            if (!writer.EndObject())
                return false;
        return true;
    }
};

template <class TJson>
struct ValueReader<TJson, ::proto::LiDARFrameData>
{
    static bool from_json(const TJson& json, ::proto::LiDARFrameData& value, const char* key = nullptr)
    {
        if (key != nullptr)
            return FBE::JSON::from_json_child(json, value, key);
        bool result = true;
        result &= FBE::JSON::from_json(json, value.screen, "screen");
        result &= FBE::JSON::from_json(json, value.newTouches, "newTouches");
        result &= FBE::JSON::from_json(json, value.removeTouches, "removeTouches");
        result &= FBE::JSON::from_json(json, value.updateTouches, "updateTouches");
        return result;
    }
};

template <class TWriter>
struct ValueWriter<TWriter, ::proto::MetaMessage>
{
    static bool to_json(TWriter& writer, const ::proto::MetaMessage& value, bool scope = true)
    {
        if (scope)
            if (!writer.StartObject())
                return false;
        if (!FBE::JSON::to_json_key(writer, "body") || !FBE::JSON::to_json(writer, value.body, true))
            return false;
        if (scope)
            if (!writer.EndObject())
                return false;
        return true;
    }
};

template <class TJson>
struct ValueReader<TJson, ::proto::MetaMessage>
{
    static bool from_json(const TJson& json, ::proto::MetaMessage& value, const char* key = nullptr)
    {
        if (key != nullptr)
            return FBE::JSON::from_json_child(json, value, key);
        bool result = true;
        result &= FBE::JSON::from_json(json, value.body, "body");
        return result;
    }
};

template <class TWriter>
struct ValueWriter<TWriter, ::proto::MetaBinaryMessage>
{
    static bool to_json(TWriter& writer, const ::proto::MetaBinaryMessage& value, bool scope = true)
    {
        if (scope)
            if (!writer.StartObject())
                return false;
        if (!FBE::JSON::to_json_key(writer, "body") || !FBE::JSON::to_json(writer, value.body, true))
            return false;
        if (scope)
            if (!writer.EndObject())
                return false;
        return true;
    }
};

template <class TJson>
struct ValueReader<TJson, ::proto::MetaBinaryMessage>
{
    static bool from_json(const TJson& json, ::proto::MetaBinaryMessage& value, const char* key = nullptr)
    {
        if (key != nullptr)
            return FBE::JSON::from_json_child(json, value, key);
        bool result = true;
        result &= FBE::JSON::from_json(json, value.body, "body");
        return result;
    }
};

template <class TWriter>
struct ValueWriter<TWriter, ::proto::LiDARMapperMessage>
{
    static bool to_json(TWriter& writer, const ::proto::LiDARMapperMessage& value, bool scope = true)
    {
        if (scope)
            if (!writer.StartObject())
                return false;
        if (!FBE::JSON::to_json_key(writer, "body") || !FBE::JSON::to_json(writer, value.body, true))
            return false;
        if (scope)
            if (!writer.EndObject())
                return false;
        return true;
    }
};

template <class TJson>
struct ValueReader<TJson, ::proto::LiDARMapperMessage>
{
    static bool from_json(const TJson& json, ::proto::LiDARMapperMessage& value, const char* key = nullptr)
    {
        if (key != nullptr)
            return FBE::JSON::from_json_child(json, value, key);
        bool result = true;
        result &= FBE::JSON::from_json(json, value.body, "body");
        return result;
    }
};

} // namespace JSON

} // namespace FBE