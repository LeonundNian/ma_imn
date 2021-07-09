/**
 * 定义ｔｆ监听器
 * 查找坐标变换
 * */
#include<ros/ros.h>
#include<tf/transform_listener.h>
#include<geometry_msgs/Twist.h>
#include<turtlesim/Spawn.h>
int main(int argc,char ** argv){
    //初始化ｒｏｓ节点和句柄　请求产生乌龟
    ros::init(argc,argv,"my_tf_listener");
    ros::NodeHandle node;
    ros::service::waitForService("/spawn");
    ros::ServiceClient add_turtle = node.serviceClient<turtlesim::Spawn>("/spawn");
    turtlesim::Spawn srv;
    add_turtle.call(srv);
    //创建发布ｔｕｒｔｌｅ速度控制指令的发布者
    ros::Publisher turtle_vel = node.advertise<geometry_msgs::Twist>("/turtle2/cmd_vel",10);
    //创建ｔｆ的监听器
    tf::TransformListener listener;
    ros::Rate rate(10.0);
    while (ros::ok())
    {
        //获取两个海龟坐标系之间的ｔｆ数据
        tf::StampedTransform transform;
        try
        {
            listener.waitForTransform("/turtle2","/turtle1",ros::Time(0),ros::Duration(3.0));
            listener.lookupTransform("/turtle2","/turtle1",ros::Time(0),transform);

        }
        catch(tf::TransformException &ex)
        {
            ROS_ERROR("%s",ex.what());
            // ros::Duration(1.0)这里代表时长
            ros::Duration(1.0).sleep();
            continue;
        }
        //根据两个乌龟之间的位置关系，发布ｔｕｒｔｌｅ的速度控制指令
        geometry_msgs::Twist vel_msg;
        //arctan(y/x);
        vel_msg.angular.z = 4.0 * atan2(
            transform.getOrigin().y(),
            transform.getOrigin().x()
        );
        vel_msg.linear.x = 0.5* sqrt(
            pow(transform.getOrigin().x(),2) +
            pow(transform.getOrigin().y(),2)
        );

        turtle_vel.publish(vel_msg);
    }
    
    return 0;
}