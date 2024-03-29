#ifndef RDF4CPP_FOAF_HPP
#define RDF4CPP_FOAF_HPP

#include <rdf4cpp/ClosedNamespace.hpp>

namespace rdf4cpp::namespaces {

/**
 * A ClosedNamespace for http://xmlns.com/foaf/0.1/ version https://xmlns.com/foaf/spec/20140114.rdf
 */
struct FOAF : ClosedNamespace {
    static constexpr std::string_view prefix = "http://xmlns.com/foaf/0.1/";
    static constexpr std::array<std::string_view, 73> suffixes = {"Person", "name", "homepage", "openid", "img", "Document",
                                                                  "Image", "mbox", "depiction", "Agent", "Group", "member",
                                                                  "workplaceHomepage", "membershipClass", "Organization",
                                                                  "OnlineAccount", "PersonalProfileDocument", "maker",
                                                                  "primaryTopic", "Project", "LabelProperty", "OnlineChatAccount",
                                                                  "account", "accountServiceHomepage", "accountName",
                                                                  "OnlineEcommerceAccount", "OnlineGamingAccount",
                                                                  "isPrimaryTopicOf", "knows", "made", "page", "weblog",
                                                                  "aimChatID", "based", "currentProject", "depicts", "familyName",
                                                                  "firstName", "focus", "gender", "givenName", "icqChatID",
                                                                  "interest", "jabberID", "lastName", "logo", "msnChatID",
                                                                  "myersBriggs", "nick", "pastProject", "phone", "plan",
                                                                  "publications", "schoolHomepage", "skypeID", "thumbnail",
                                                                  "tipjar", "title", "topic", "workInfoHomepage", "yahooChatID",
                                                                  "age", "birthday", "sha", "status", "dnaChecksum", "family",
                                                                  "fundedBy", "geekcode", "givenname", "holdsAccount", "surname", "theme"};

    explicit FOAF(storage::DynNodeStoragePtr node_storage = storage::default_node_storage) : ClosedNamespace{prefix, suffixes, node_storage} {
    }
};

}  // namespace rdf4cpp::namespaces

#endif  //RDF4CPP_FOAF_HPP
