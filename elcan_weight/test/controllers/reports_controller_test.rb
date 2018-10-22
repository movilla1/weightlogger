require 'test_helper'

class ReportsControllerTest < ActionController::TestCase
  test "should get index" do
    get :index
    assert_response :success
  end

  test "should get drivers" do
    get :drivers
    assert_response :success
  end

  test "should get trucks" do
    get :trucks
    assert_response :success
  end

end
