# frozen_string_literal: true

Rails.application.routes.draw do
  devise_for :usuarios, ActiveAdmin::Devise.config
  ActiveAdmin.routes(self)
  get '/about', to: 'home#about'
  get '/help', to: 'home#help'
  post '/pesaje/create_from_rfid', to: 'pesaje#create_from_rfid',
                                   as: :pesaje_create_from_rfid
  resources :pesaje, only: %w[new create]
  root to: 'home#index'
end
