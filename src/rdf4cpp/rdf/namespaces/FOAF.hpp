#ifndef RDF4CPP_FOAF_HPP
#define RDF4CPP_FOAF_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://xmlns.com/foaf/0.1/ version https://xmlns.com/foaf/spec/20140114.rdf
 */
class FOAF : public ClosedNamespace {
public:
    static constexpr std::string_view NAMESPACE = "http://xmlns.com/foaf/0.1/";
    static constexpr std::array<std::string_view, 73> SUFFIXES = {"Person", "name", "homepage", "openid", "img", "Document", "Image", "mbox", "depiction", "Agent", "Group", "member", "workplaceHomepage", "membershipClass", "Organization", "OnlineAccount", "PersonalProfileDocument", "maker", "primaryTopic", "Project", "LabelProperty", "OnlineChatAccount", "account", "accountServiceHomepage", "accountName", "OnlineEcommerceAccount", "OnlineGamingAccount", "isPrimaryTopicOf", "knows", "made", "page", "weblog", "aimChatID", "based", "currentProject", "depicts", "familyName", "firstName", "focus", "gender", "givenName", "icqChatID", "interest", "jabberID", "lastName", "logo", "msnChatID", "myersBriggs", "nick", "pastProject", "phone", "plan", "publications", "schoolHomepage", "skypeID", "thumbnail", "tipjar", "title", "topic", "workInfoHomepage", "yahooChatID", "age", "birthday", "sha", "status", "dnaChecksum", "family", "fundedBy", "geekcode", "givenname", "holdsAccount", "surname", "theme"};

    explicit FOAF(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_FOAF_HPP
