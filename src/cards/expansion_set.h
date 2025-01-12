#ifndef __EXPANSION_SET_H__
#define __EXPANSION_SET_H__

#include "cards/card_data.h"

#include "utils/parse_string.h"

namespace banggame {

    inline bool validate_expansions(const expansion_set &expansions) {
        return rn::all_of(expansions, [&](banggame::ruleset_ptr ruleset) {
            return ruleset->is_valid_with(expansions);
        });
    }

}

namespace json {

    template<typename Context> struct serializer<banggame::ruleset_ptr, Context> {
        json operator()(banggame::ruleset_ptr value) const {
            return value->name;
        }
    };

    template<typename Context> struct deserializer<banggame::expansion_set, Context> {
        banggame::expansion_set operator()(const json &value) const {
            if (!value.is_array()) {
                throw deserialize_error("Cannot deserialize expansion_set");
            }

            banggame::expansion_set result;

            for (const auto &elem : value) {
                if (!elem.is_string()) {
                    throw deserialize_error("Cannot deserialize ruleset_ptr");
                }

                auto name = elem.get<std::string_view>();
                for (banggame::ruleset_ptr expansion : banggame::all_cards.expansions) {
                    if (expansion->name == name) {
                        result.insert(expansion);
                        break;
                    }
                }
            }

            return result;
        }
    };

}

namespace std {

    template<> struct formatter<banggame::expansion_set> : formatter<std::string_view> {
        static string expansions_to_string(banggame::expansion_set value) {
            std::string ret;
            for (banggame::ruleset_ptr ruleset : banggame::all_cards.expansions) {
                if (value.contains(ruleset)) {
                    if (!ret.empty()) {
                        ret += ' ';
                    }
                    ret.append(ruleset->name);
                }
            }
            return ret;
        }

        auto format(banggame::expansion_set value, format_context &ctx) const {
            return formatter<std::string_view>::format(expansions_to_string(value), ctx);
        }
    };

}

namespace utils {

    template<> struct string_parser<banggame::expansion_set> {
        std::optional<banggame::expansion_set> operator()(std::string_view str) {
            constexpr std::string_view whitespace = " \t";
            banggame::expansion_set result;
            while (true) {
                size_t pos = str.find_first_not_of(whitespace);
                if (pos == std::string_view::npos) break;
                str = str.substr(pos);
                pos = str.find_first_of(whitespace);
                auto it = rn::find(banggame::all_cards.expansions, str.substr(0, pos), &banggame::ruleset_vtable::name);
                if (it != banggame::all_cards.expansions.end()) {
                    result.insert(*it);
                } else {
                    return std::nullopt;
                }
                if (pos == std::string_view::npos) break;
                str = str.substr(pos);
            }
            if (banggame::validate_expansions(result)) {
                return result;
            }
            return std::nullopt;
        }
    };

}

#endif