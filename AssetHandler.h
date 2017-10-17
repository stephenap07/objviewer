#pragma once

#include <memory>
#include <unordered_map>

template <typename T>
class AssetHandler
{
public:
   template <class... Args>
   void emplace(const std::string& name, Args&&... args)
   {
      objects_.emplace(std::make_pair<std::string, std::shared_ptr<T>>(
         name, std::make_shared<T>(std::forward<Args>(args)...)));
   }

   std::shared_ptr<T> get(const std::string& name)
   {
      auto f = objects_.find(name);
      if (f != objects_.end())
      {
         return f->second;
      }
      return std::shared_ptr<T>();
   }

private:
   std::unordered_map<std::string, std::shared_ptr<T>> objects_;
};
