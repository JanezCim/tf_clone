#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <iostream>
#include <fstream>


using namespace std;

int main(int argc, char** argv){
  ros::init(argc, argv, "tf_clone_node");
  ros::NodeHandle node("~");

  // //#####################################ROS PARAMS################################################################ 
  string input_child_frame;
  string input_parent_frame;
  if(!node.getParam("input_parent_frame", input_parent_frame)){
    ROS_ERROR("input_parent_frame must be set");
    return 0;
  }
  if(!node.getParam("input_child_frame", input_child_frame)){
    ROS_ERROR("input_child_frame must be set");
    return 0;
  }
  
  string output_child_frame;
  string output_parent_frame;
  if(!node.getParam("output_parent_frame", output_parent_frame)){
    ROS_ERROR("output_parent_frame must be set");
    return 0;
  }
  if(!node.getParam("output_child_frame", output_child_frame)){
    ROS_ERROR("output_child_frame must be set");
    return 0;
  }

  double frequency;
  node.param("frequency", frequency, 15.0);
  
  // //#############################################################################################################


  ROS_INFO("Cloning input transform %s -> %s \n with transformation to output transform %s -> %s",  input_parent_frame.c_str(),
                                                                                                    input_child_frame.c_str(),
                                                                                                    output_parent_frame.c_str(),
                                                                                                    output_child_frame.c_str());

  tf2_ros::Buffer tfBuffer;
  tf2_ros::TransformListener tfListener(tfBuffer);
  tf2_ros::TransformBroadcaster br;

  // innit the vars used in the program
  bool print_logging_msg = false;
  ros::Time last_msg_time = ros::Time::now();

  // main loop
  ros::Rate rate(frequency);
  while (node.ok()){
    // Innit the transforms
    geometry_msgs::TransformStamped inTransform;
    geometry_msgs::TransformStamped outTransform;
    
    try{
      inTransform = tfBuffer.lookupTransform(input_parent_frame, input_child_frame, ros::Time(0));

      if(inTransform.header.stamp > last_msg_time){
        outTransform.header.frame_id = output_parent_frame;
        outTransform.header.seq = inTransform.header.seq;
        outTransform.header.stamp = inTransform.header.stamp;
        outTransform.child_frame_id = output_child_frame;

        outTransform.transform.translation.x = inTransform.transform.translation.x;
        outTransform.transform.translation.y = inTransform.transform.translation.y;
        outTransform.transform.translation.z = inTransform.transform.translation.z;

        outTransform.transform.rotation.w = inTransform.transform.rotation.w;
        outTransform.transform.rotation.x = inTransform.transform.rotation.x;
        outTransform.transform.rotation.y = inTransform.transform.rotation.y;
        outTransform.transform.rotation.z = inTransform.transform.rotation.z;

        br.sendTransform(outTransform);

        last_msg_time = inTransform.header.stamp;
      }
  
    }
    catch (tf2::TransformException &ex) {
      ROS_WARN("%s",ex.what());
      ros::Duration(1.0).sleep();
      print_logging_msg = true;
      continue;
    }
    
    rate.sleep();
  }
  
  return 0;
};