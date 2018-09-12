# frozen_string_literal: true

class Truck < ActiveRecord::Base
  has_many :users
end
