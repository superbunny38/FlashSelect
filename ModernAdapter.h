#pragma once
#include "LegacySystem.h"
#include <string_view>
#include <atomic>

class ModernItem{
    public:
        ModernItem() = default;
    //We hold a pointer to the legacy item so we can modify quota later
        explicit ModernItem(LegacyItem* legacy): legacy_ptr_(legacy){
            if (legacy){
            id_ = (legacy->id != nullptr)? std::string_view(legacy->id) : std::string_view{};
            value_ = legacy->value;}
        }
        std::string_view GetID() const {
            return id_;
        }
        int GetValue() const {
            return value_;}
        int GetQuotaRemaining() const {
            return legacy_ptr_ ? legacy_ptr_->quota_remaining : 0;
        }

        //Thread-safe decrement of quota (not compatible with macOS though
        //std::atoic_ref: to treat the raw int as atomic
        // void DecrementQuota(){
        //     if (legacy_ptr_){
        //         std::atomic_ref<int> atomic_quota(legacy_ptr_->quota_remaining);
        //         atomic_quota.fetch_sub(1, std::memory_order_relaxed);
        //     }
        // }

        bool DecrementQuota(){
            if (!legacy_ptr_){
                return false;
            }

            auto* atomic_ptr = reinterpret_cast<std::atomic<int>*>(&(legacy_ptr_->quota_remaining));

            int current = atomic_ptr->load();

            while (current > 0){
                if (atomic_ptr->compare_exchange_weak(current, current - 1)){
                    return true;
                }
            }
            return false;
        }

    private:
        LegacyItem* legacy_ptr_ = nullptr;
        std::string_view id_;
        int value_ = 0;
};