#include "FOAF.hpp"

namespace rdf4cpp::rdf::namespaces {

const std::string FOAF::NAMESPACE = "http://xmlns.com/foaf/0.1/";
const std::vector<std::string> FOAF::SUFFIXES = {"Person", "name", "homepage", "openid", "img", "Document", "Image", "mbox", "depiction", "Agent", "Group", "member", "workplaceHomepage", "membershipClass", "Organization", "OnlineAccount", "PersonalProfileDocument", "maker", "primaryTopic", "Project", "LabelProperty", "OnlineChatAccount", "account", "accountServiceHomepage", "accountName", "OnlineEcommerceAccount", "OnlineGamingAccount", "isPrimaryTopicOf", "knows", "made", "page", "weblog", "aimChatID", "based", "currentProject", "depicts", "familyName", "firstName", "focus", "gender", "givenName", "icqChatID", "interest", "jabberID", "lastName", "logo", "msnChatID", "myersBriggs", "nick", "pastProject", "phone", "plan", "publications", "schoolHomepage", "skypeID", "thumbnail", "tipjar", "title", "topic", "workInfoHomepage", "yahooChatID", "age", "birthday", "sha", "status", "dnaChecksum", "family", "fundedBy", "geekcode", "givenname", "holdsAccount", "surname", "theme"};
FOAF::FOAF(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces