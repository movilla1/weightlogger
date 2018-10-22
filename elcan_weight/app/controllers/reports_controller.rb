class ReportsController < ApplicationController
  def index
    #this shows the index report
  end

  def drivers
    if params[:driver_id].present?
      @rpt = create_report_by_driver(params[:driver_id])
      send_file @rpt.filename, @rpt.file_content
    end
  end

  def trucks
    @rpt = create_report_by_truck(params[:truck_id])
    send_file @rpt.filename, @rpt.file_content
  end
end
