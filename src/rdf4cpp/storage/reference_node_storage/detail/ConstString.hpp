#ifndef RDF4CPP_REFERENCENODESTORAGE_CONSTSTRING_HPP
#define RDF4CPP_REFERENCENODESTORAGE_CONSTSTRING_HPP

#include <string>

namespace rdf4cpp::storage::reference_node_storage::detail {

    /**
     * A constant-size (i.e. non-growing), heap-allocated string type
     * Behaves like std::string except that it cannot grow and therefore occupies 1 less word in memory (it has no capacity field).
     */
    template<typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    struct BasicConstString {
        using value_type = Char;
        using traits_type = Traits;
        using allocator_type = Allocator;
        using size_type = size_t;
        using different_type = std::ptrdiff_t;
        using pointer = typename std::allocator_traits<allocator_type>::pointer;
        using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    private:
        pointer data_;
        size_type size_;
        [[no_unique_address]] allocator_type alloc_;

        static constexpr void swap_data(BasicConstString &a, BasicConstString &b) noexcept {
            std::swap(a.data_, b.data_);
            std::swap(a.size_, b.size_);
        }

    public:
        explicit BasicConstString(std::string_view const sv,
                                  allocator_type const &alloc = allocator_type{}) : size_{sv.size()},
                                                                                    alloc_{alloc} {
            data_ = std::allocator_traits<allocator_type>::allocate(alloc_, size_);
            memcpy(data_, sv.data(), size_);
        }

        // prevent copy constructors from being accidentally called
        BasicConstString(BasicConstString const &) = delete;
        BasicConstString &operator=(BasicConstString const &) = delete;

        BasicConstString(BasicConstString &&other) noexcept : data_{std::exchange(other.data_, nullptr)},
                                                              size_{std::exchange(other.size_, 0)},
                                                              alloc_{std::move(other.alloc_)} {
        }

        BasicConstString &operator=(BasicConstString &&other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value
                                                                            || std::allocator_traits<Allocator>::is_always_equal::value) {
            assert(this != &other);

            if constexpr (std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
                swap(*this, other);
                return *this;
            } else if constexpr (std::allocator_traits<Allocator>::is_always_equal::value) {
                swap_data(*this, other);
                return *this;
            } else {
                if (alloc_ == other.alloc_) [[likely]] {
                    swap_data(*this, other);
                    return *this;
                }

                // alloc_ != other.alloc_ and not allowed to propagate, need to copy

                if (size_ != other.size_) [[likely]] {
                    std::allocator_traits<allocator_type>::deallocate(alloc_, data_, size_);
                    size_ = other.size_;
                    data_ = std::allocator_traits<allocator_type>::allocate(alloc_, size_);
                }

                memcpy(data_, other.data_, size_);
                return *this;
            }
        }

        ~BasicConstString() {
            if (data_ != nullptr) {
                std::allocator_traits<allocator_type>::deallocate(alloc_, data_, size_);
            }
        }

        [[nodiscard]] value_type const *data() const noexcept {
            return data_;
        }

        [[nodiscard]] size_type size() const noexcept {
            return size_;
        }

        operator std::basic_string_view<value_type, traits_type>() const noexcept {
            return {data_, size_};
        }

        friend void swap(BasicConstString &a, BasicConstString &b) noexcept {
            std::swap(a.data_, b.data_);
            std::swap(a.size_, b.size_);
            std::swap(a.alloc_, b.alloc_);
        }
    };

    using ConstString = BasicConstString<char>;

} // namespace rdf4cpp::storage::reference_node_storage::detail

#endif // RDF4CPP_REFERENCENODESTORAGE_CONSTSTRING_HPP
