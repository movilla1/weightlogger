Rails.application.routes.draw do
  devise_for :usuarios, ActiveAdmin::Devise.config
  ActiveAdmin.routes(self)
  resource :pesaje, only: %w[new create]

  root to: 'home#index'
end
