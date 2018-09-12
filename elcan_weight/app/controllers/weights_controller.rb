# frozen_string_literal: true

class WeightController < ApplicationController
  def new; end

  def create; end

  def create_from_rfid
    data = params[:data]
    tag_id = data[0]
    time_part = data[1, 4]
    weight = data[5, 12]
    device = data[13]
    user = User.find_tag(tag_id: tag_id)
    return false if user.blank?
    tmp_truck = user.truck
    truck_id = tmp_truck.id
    Weight.create(truck_id: truck_id, created_at: time_part,
                  eje: device, peso: weight)
    render json: 'OK', status: 200
  end
end
