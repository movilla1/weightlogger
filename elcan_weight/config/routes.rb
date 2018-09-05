Rails.application.routes.draw do
  devise_for :usuarios
  root to: "home#index"
end
