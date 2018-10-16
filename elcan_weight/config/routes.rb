# frozen_string_literal: true

Rails.application.routes.draw do
  devise_for :users, ActiveAdmin::Devise.config
  ActiveAdmin.routes(self)
  get '/about', to: 'home#about'
  get '/help', to: 'home#help'
  post '/pesaje/create_from_rfid', to: 'weights#create_from_rfid',
                                   as: :weights_create_from_rfid
  resources :weight, only: %w[new create]
  root to: 'home#index'
end
