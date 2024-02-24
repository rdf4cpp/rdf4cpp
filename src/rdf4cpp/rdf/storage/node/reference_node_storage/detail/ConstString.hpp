#ifndef RDF4CPP_REFERENCENODESTORAGE_CONSTSTRING_HPP
#define RDF4CPP_REFERENCENODESTORAGE_CONSTSTRING_HPP

#include <string>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

    template<typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    struct BasicConstString {
        using value_type = Char;
        using traits_type = std::char_traits<value_type>;
        using allocator_type = Allocator;
        using size_type = size_t;
        using different_type = std::ptrdiff_t;
        using pointer = std::allocator_traits<allocator_type>::pointer;
        using const_pointer = std::allocator_traits<allocator_type>::const_pointer;

    private:
        pointer data_;
        size_type size_;
        [[no_unique_address]] allocator_type alloc_;

        void drop() noexcept {
            if (data_ == nullptr) {
                return;
            }

            std::allocator_traits<allocator_type>::deallocate(alloc_, data_, size_);
        }

    public:
        explicit BasicConstString(std::string_view const sv, allocator_type const &alloc = allocator_type{}) : alloc_{alloc} {
            size_ = sv.size();
            data_ = std::allocator_traits<allocator_type>::allocate(alloc_, size_);
            memcpy(data_, sv.data(), size_);
        }

        BasicConstString(BasicConstString const &) = delete;

        BasicConstString(BasicConstString &&other) noexcept : data_{std::exchange(other.data_, nullptr)},
                                                              size_{std::exchange(other.size_, 0)},
                                                              alloc_{std::move(other.alloc_)} {
        }

        BasicConstString &operator=(BasicConstString const &) = delete;

        BasicConstString &operator=(BasicConstString &&other) noexcept {
            assert(this != &other);

            drop();
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
            alloc_ = std::move(other.alloc_);

            return *this;
        }

        ~BasicConstString() noexcept {
            drop();
        }

        [[nodiscard]] value_type const *data() const noexcept {
            return data_;
        }

        [[nodiscard]] size_type size() const noexcept {
            return size_;
        }

        operator std::string_view() const noexcept {
            return std::string_view{data_, size_};
        }
    };

    using ConstString = BasicConstString<char>;

} // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif // RDF4CPP_REFERENCENODESTORAGE_CONSTSTRING_HPP
