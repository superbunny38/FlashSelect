#pragma once
#include "LegacySystem.h"
#include <string_view>


class ModernItem{
    public:
    //We hold a pointer to the legacy item so we can modify quota later
        explicit ModernItem(LegacySystem* legacy): legacy_ptr_(legacy){
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

        void DecrementQuota(){
            if (legacy_ptr_){
                legacy_ptr_->quota_remaining--;
            }
        }

    private:
        LegacySystem* legacy_ptr_ = nullptr;
        std::string_view id_;
        int value_ = 0;
};